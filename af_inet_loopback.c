#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "decls.h"

typedef struct state {
  int ready_fd;
  int listen_sockfd;
  int peer_sockfd;
  struct sockaddr_in listen_addr;
} state;

state *new_state() { return calloc(sizeof(state), 1); }

void free_state(state *state) { free(state); }

int pre_fork_setup(state *state) {
  state->listen_addr.sin_family = AF_INET;
  state->listen_addr.sin_port = htons(0);
  state->listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  socklen_t addr_len = sizeof(struct sockaddr_in);

  if ((state->ready_fd = eventfd(0 /* initval */, 0 /* flags */)) == -1) {
    // uh-oh
  }
  if ((state->listen_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    // uh-oh
  }
  if (bind(state->listen_sockfd, (struct sockaddr *)&state->listen_addr,
           sizeof(struct sockaddr_in))) {
    // uh-oh
  }
  if (getsockname(state->listen_sockfd, (struct sockaddr *)&state->listen_addr,
                  &addr_len) ||
      addr_len != sizeof(struct sockaddr_in)) {
    // uh-oh
  }
  return 0;
}

int cleanup(state *state __attribute__((unused))) { return 0; }

int child_post_fork_setup(state *state) {
  listen(state->listen_sockfd, 1);
  write(state->ready_fd, "\0\0\0\0\0\0\0\1", 8);  // Signal parent we're ready.
  if ((state->peer_sockfd = accept(state->listen_sockfd, NULL, NULL)) == -1) {
    // uh-oh
  }
  return 0;
}

int child_warmup(int warmup_iters __attribute__((unused)),
                 state *state __attribute__((unused))) {
  return 0;
}

int child_loop(int iters __attribute__((unused)), state *state) {
  for (;;) {
    char msg;
    read(state->peer_sockfd, &msg, 1);
    write(state->peer_sockfd, "1", 1);
  }
  return 0;
}

int child_cleanup(state *state) {
  close(state->listen_sockfd);
  return 0;
}

int parent_post_fork_setup(state *state __attribute__((unused))) {
  close(state->listen_sockfd);
  char msg[8];
  read(state->ready_fd, msg, 8);
  if ((state->peer_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    // uh-oh
  }
  if (connect(state->peer_sockfd, (struct sockaddr *)&state->listen_addr,
              sizeof(struct sockaddr_in))) {
    // uh-oh
  }
  return 0;
}

int parent_warmup(int warmup_iters, state *state) {
  for (int i = 0; i < warmup_iters; ++i) {
    char resp;
    if (write(state->peer_sockfd, "0", 1) == -1) {
      perror("could not write");
      break;
    };

    if (read(state->peer_sockfd, &resp, 1) == -1) {
      perror("could not read");
      break;
    }
  }
  return 0;
}

int parent_loop(int iters, state *state) {
  for (int i = 0; i < iters; ++i) {
    char resp;
    if (write(state->peer_sockfd, "0", 1) == -1) {
      perror("could not write");
      break;
    };

    if (read(state->peer_sockfd, &resp, 1) == -1) {
      perror("could not read");
      break;
    }
  }
  return 0;
}

int parent_cleanup(state *state) {
  close(state->peer_sockfd);

  return 0;
}
