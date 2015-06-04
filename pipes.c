#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_ITERS 100000
#define WARMUP_ITERS 10000

void parent_loop(long iters, int tx_fd, int rx_fd) {
    int i;
    struct timespec start, end;

    fprintf(stderr, "About to start!\n");

    for (i = 0; i < WARMUP_ITERS; ++i) {
        char resp;
        if (write(tx_fd, "0", 1) == -1) {
            perror("could not write");
            break;
        };

        if (read(rx_fd, &resp, 1) == -1) {
            perror("could not read");
            break;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (i = 0; i < iters; ++i) {
        char resp;
        if (write(tx_fd, "0", 1) == -1) {
            perror("could not write");
            break;
        };

        if (read(rx_fd, &resp, 1) == -1) {
            perror("could not read");
            break;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    fprintf(stderr, "Started at %lld.%ld\n", (long long) start.tv_sec, start.tv_nsec);
    fprintf(stderr, "Ended at %lld.%ld\n", (long long) end.tv_sec, end.tv_nsec);

    long long elapsed_nsec = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);

    fprintf(stderr, "%ld iters in %lld ns\n %f ns/iter\n", iters, elapsed_nsec, (double) elapsed_nsec / iters);

    exit(EXIT_SUCCESS);
}

void child_loop(int tx_fd, int rx_fd) {
    for (;;) {
        char msg;
        read(rx_fd, &msg, 1);
        write(tx_fd, "1", 1);
    }
}

int main(int argc, char **argv) {
    long iters;

    if (argc > 1) {
        errno = 0;
        iters = strtol(argv[1], NULL, 10);
        if (errno) {
            perror("if provided, first argument must be an integer");
            exit(EXIT_FAILURE);
        }
    } else {
        iters = DEFAULT_ITERS;
    }

    fprintf(stderr, "iters: %ld\n", iters);

    int tx[2], rx[2];  // relative to parent
    if (pipe(tx)) {
        // uh-oh
    }
    if (pipe(rx)) {
        // uh-oh
    }

    pid_t child = fork();
    if (child == -1) {
        // uh-oh
    }
    else if (child) {
        close(tx[0]);
        close(rx[1]);

        parent_loop(iters, tx[1], rx[0]);
    } else {
        close(tx[1]);
        close(rx[0]);

        child_loop(rx[1], tx[0]);
    }

    return 0;
}
