#include <stdio.h>
#include <stdlib.h>
#include <sys/eventfd.h>
#include <unistd.h>

typedef struct state {
    int tx, rx;  // relative to parent
} state;

state* new_state() {
    return calloc(sizeof(state), 1);
}

void free_state(state* state) {
    free(state);
}

int pre_fork_setup(state* state) {
    if ((state->tx = eventfd(0 /* initval */, 0 /* flags */)) == -1) {
        // uh-oh
    }
    if ((state->rx = eventfd(0 /* initval */, 0 /* flags */)) == -1) {
        // uh-oh
    }
    return 0;
}

int cleanup(state* state __attribute__((unused))) {
    return 0;
}

int child_post_fork_setup(state* state __attribute__((unused))) {
    return 0;
}

int child_warmup(int warmup_iters __attribute__((unused)), state* state __attribute__((unused))) {
    return 0;
}

int child_loop(int warmup_iters __attribute__((unused)), state* state) {
    for (;;) {
        char msg[8];
        read(state->tx, msg, 8);
        write(state->rx, "\0\0\0\0\0\0\0\1", 8);
    }
    return 0;
}

int child_cleanup(state* state __attribute__((unused))) {
    return 0;
}

int parent_post_fork_setup(state* state __attribute__((unused))) {
    return 0;
}

int parent_warmup(int warmup_iters, state* state) {
    int i;

    for (i = 0; i < warmup_iters; ++i) {
        char resp[8];
        if (write(state->tx, "\0\0\0\0\0\0\0\1", 8) == -1) {
            perror("could not write");
            break;
        };

        if (read(state->rx, &resp, 8) == -1) {
            perror("could not read");
            break;
        }
    }
    return 0;
}

int parent_loop(int iters, state* state) {
    int i;

    for (i = 0; i < iters; ++i) {
        char resp[8];
        if (write(state->tx, "\0\0\0\0\0\0\0\1", 8) == -1) {
            perror("could not write");
            break;
        };

        if (read(state->rx, &resp, 8) == -1) {
            perror("could not read");
            break;
        }
    }
    return 0;
}

int parent_cleanup(state* state __attribute__((unused))) {
    return 0;
}
