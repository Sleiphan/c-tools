#include <stdio.h>
#include <liburing.h>
#include <string.h>
#include <unistd.h>

int test_file_read(const char* file_path) {
    int return_code = 0;

    struct io_uring ring;
    struct io_uring_sqe* sqe;
    struct io_uring_cqe* cqe;

    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE + 1];

    memset(buffer, 0, (BUFFER_SIZE + 1) * sizeof(char));

    const int uring_init_status = io_uring_queue_init(8, &ring, 0);

    if (uring_init_status < 0) {
        perror("io_uring_queue_init");
        return_code = -1;
        goto exit_uring;
    }
    printf("Successfully created an io_uring\n");

    // Open file
    sqe = io_uring_get_sqe(&ring);
    io_uring_prep_open(sqe, file_path, O_RDONLY, 0);
    io_uring_submit(&ring);

    // Wait for OPEN operation to complete
    io_uring_wait_cqe(&ring, &cqe);

    // Exit if the file could not be opened
    if (cqe->res < 0) {
        io_uring_cqe_seen(&ring, cqe);
        fprintf(stderr, "Open failed: %s\n", strerror(-cqe->res));
        return_code = -1;
        goto exit_uring;
    }

    const int fd = cqe->res;
    io_uring_cqe_seen(&ring, cqe);

    // Read the file
    sqe = io_uring_get_sqe(&ring);
    io_uring_prep_read(sqe, fd, buffer, BUFFER_SIZE, 0L);
    io_uring_submit(&ring);

    // Wait for the READ operation to complete
    io_uring_wait_cqe(&ring, &cqe);

    // Exit if the read operation failed
    if (cqe->res < 0) {
        fprintf(stderr, "read() failed: %s\n", strerror(-cqe->res));
        io_uring_cqe_seen(&ring, cqe);
        close(fd);
        return_code = -1;
        goto exit_uring;
    }
    io_uring_cqe_seen(&ring, cqe);

    // Make sure that the buffer is delimited
    buffer[cqe->res] = '\0';

    // Print the buffer
    printf("%s\n", buffer);

    // Close the file
    sqe = io_uring_get_sqe(&ring);
    io_uring_prep_close(sqe, fd);
    io_uring_submit(&ring);

    // Wait until the CLOSE operation has completed
    io_uring_wait_cqe(&ring, &cqe);
    if (cqe->res < 0) {
        printf("close() failed: %s\n", strerror(errno));
        return_code = -1;
    }

    // The CLOSE operation has been processed
    io_uring_cqe_seen(&ring, cqe);

    exit_uring:
    io_uring_queue_exit(&ring);

    return return_code;
}

int main() {
    const char file_path[] = "file.txt";
    test_file_read(file_path);

    return 0;
}