#include <stdio.h>
#include <liburing.h>

int main() {
    printf("Hello world!\n");

    struct io_uring ring;
    int io_uring_setup_status = io_uring_queue_init(8, &ring, 0);

    if (io_uring_setup_status < 0) {
        perror("io_uring_queue_init");
        return 1;
    }
    printf("Successfully set up io_uring!\n");

    io_uring_queue_exit(&ring);

    return 0;
}