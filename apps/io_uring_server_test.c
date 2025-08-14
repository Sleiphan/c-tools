#include <stdio.h>
#include <stdlib.h>
#include <liburing.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>



int create_io_ring(struct io_uring* ring) {
    int result = io_uring_queue_init(1 << 8, ring, 0);

    if (result < 0)
        perror("io_uring_queue_init");
    
    return result;
}

int create_server_socket(int port_number, int* server_fd_output) {
    int err = 0;

    int server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (server_fd < 0) {
        perror("socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)");
        return server_fd;
    }

    // Allow other sockets to listen to the same port,
    // thus enabling OS load balancing between such sockets
    const int true_val = 1;
    err = setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &true_val, sizeof(true_val));
    if (err) {
        perror("setsockopt");
        goto create_server_socket_end;
    }

    // Bind the socket to any network interface or address on the specified port number
    struct sockaddr_in serverAddr;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_number);
    err = bind(server_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (err) {
        perror("bind()");
        goto create_server_socket_end;
    }

    // Mark the socket as a passive socket that will be used in subsequent accept() calls
    err = listen(server_fd, 512);

    create_server_socket_end:
    if (err)
        close(server_fd);
    else
        *server_fd_output = server_fd;

    return err;
}



typedef struct client_t {
    int sock_fd;
    int in_buffer_size;
    char in_buffer[4088];
} client_t;

client_t* client_create(int sock_fd) {
    client_t* new_client = malloc(sizeof(client_t));
    new_client->sock_fd = sock_fd;
    new_client->in_buffer_size = sizeof(new_client->in_buffer);

    return new_client;
}

void client_destroy(client_t* client) {
    free(client);
}

enum sysop_t {
    SYSOP_ACCEPT,
    SYSOP_CLOSE,
    SYSOP_READ,
    SYSOP_WRITE,
};

typedef struct {
    client_t * client;
    enum sysop_t operation;
} client_op_t;

client_op_t* client_op_create(client_t* client, enum sysop_t operation) {
    client_op_t* client_op = malloc(sizeof(client_op_t));
    client_op->client = client;
    client_op->operation = operation;

    return client_op;
}

void client_op_destroy(client_op_t* client_op) {
    free(client_op);
}



int start_listening(struct io_uring* ring, int* server_socket_output, client_op_t** accept_op_output) {
    int server_fd;
    if (create_server_socket(8080, &server_fd))
        return -1;
    
    *accept_op_output = client_op_create(client_create(server_fd), SYSOP_ACCEPT);

    struct io_uring_sqe* sqe = io_uring_get_sqe(ring);
    io_uring_prep_multishot_accept(sqe, server_fd, NULL, NULL, SOCK_NONBLOCK);
    io_uring_sqe_set_data(sqe, *accept_op_output);
    io_uring_submit(ring);

    *server_socket_output = server_fd;
    return 0;
}

void process_listener(struct io_uring_cqe* cqe, struct io_uring* ring) {
    if (cqe->res < 0) {
        fprintf(stderr, "ERROR - accept(): %s\n", strerror(-cqe->res));
        return;
    }
    
    client_t* new_client = client_create(cqe->res);

    struct io_uring_sqe* sqe;
    
    sqe = io_uring_get_sqe(ring);
    io_uring_prep_read(sqe, new_client->sock_fd, new_client->in_buffer, new_client->in_buffer_size - 1, 0L);
    io_uring_sqe_set_data(sqe, client_op_create(new_client, SYSOP_READ));
}

void process_client(struct io_uring_cqe* cqe, struct io_uring* ring) {
    const char response[] = "\
HTTP/1.1 200 OK\r\n\
Content-Type: text/plain; charset=utf-8\r\n\
Content-Length: 12\r\n\
Connection: close\r\n\
\r\n\
Hello world!";

    const unsigned int response_length = sizeof(response) / sizeof(char);

    client_op_t* op = (client_op_t*) cqe->user_data;
    client_t* client = op->client;

    struct io_uring_sqe* sqe;
    
    switch (op->operation) {
        case SYSOP_READ:
            // Write
            memcpy(client->in_buffer, response, response_length * sizeof(char));
            sqe = io_uring_get_sqe(ring);
            io_uring_prep_write(sqe, client->sock_fd, client->in_buffer, response_length - 1, 0L);
            io_uring_sqe_set_data(sqe, client_op_create(client, SYSOP_WRITE));
            io_uring_sqe_set_flags(sqe, IOSQE_IO_HARDLINK);

            // Close
            sqe = io_uring_get_sqe(ring);
            io_uring_prep_close(sqe, client->sock_fd);
            io_uring_sqe_set_data(sqe, client_op_create(client, SYSOP_CLOSE));
            break;
        case SYSOP_CLOSE:
            // Wipe the client from memory when the socket is closed.
            client_destroy(client);
            break;
        case SYSOP_WRITE:
        default:
            break;
    }
}

typedef struct {
    struct io_uring* ring;
    int server_socket_fd;
} processing_loop_args;

void* processing_loop(void* _) {
    struct io_uring ring;
    if (create_io_ring(&ring) < 0)
        goto processing_loop_end;

    int server_fd = 0;
    client_op_t* accept_op;
    if (start_listening(&ring, &server_fd, &accept_op) < 0)
        goto processing_loop_exit_ring;
    
    struct io_uring_cqe* cqe;
    bool running = true;

    while (running) {
        io_uring_wait_cqe(&ring, &cqe);
        
        while (io_uring_peek_cqe(&ring, &cqe) == 0) {
            client_op_t* client_op = (client_op_t*) cqe->user_data;
            client_t* client = client_op->client;
            
            if (client->sock_fd == server_fd)
                process_listener(cqe, &ring);
            else {
                process_client(cqe, &ring);
                client_op_destroy(client_op);
            }
            
            io_uring_cqe_seen(&ring, cqe);
        }

        io_uring_submit(&ring);
    }

    close(server_fd);
    client_op_destroy(accept_op);

    processing_loop_exit_ring:
    io_uring_queue_exit(&ring);

    processing_loop_end:
    return NULL;
}

int run_server_example() {
    int return_code = 0;

    pthread_t processing_thread;
    pthread_create(&processing_thread, NULL, processing_loop, NULL);
    pthread_join(processing_thread, NULL);

    return return_code;
}

int main() {
    return run_server_example();
}