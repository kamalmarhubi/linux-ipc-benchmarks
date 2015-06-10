/* _GNU_SOURCE is required for CPU_SET macros in sched.h */
#define _GNU_SOURCE

#include <argp.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "decls.h"

#define xstr(s) str(s)
#define str(s) #s

#define DEFAULT_CPU -1
#define DEFAULT_MEASUREMENTS 1
#define DEFAULT_ITERS 100000
#define DEFAULT_WARMUP_ITERS 1000

static struct argp_option options[] = {
    {"child-cpu", 'c', "CPUID", 0,
     "CPU to run the child on; default is to let the scheduler do as it will",
     0},
    {"parent-cpu", 'p', "CPUID", 0,
     "CPU to run the parent on; default is to let the scheduler do as it will",
     0},
    {"repeat", 'r', "COUNT", 0,
     "number of times to repeat measurement; default: " xstr(
         DEFAULT_MEASUREMENTS),
     0},
    {"iters", 'i', "COUNT", 0,
     "number of iterations to measure; default: " xstr(DEFAULT_ITERS), 0},
    {0, 'n', "COUNT", OPTION_ALIAS, 0, 0},
    {"warmup-iters", 'w', "COUNT", 0,
     "number of iterations before measurement; default: " xstr(
         DEFAULT_WARMUP_ITERS),
     0},
    {0}};

typedef struct {
  int child_cpu;
  int parent_cpu;
  int iters;
  int repeats;
  int warmup_iters;
} args;

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  args *args = state->input;

  switch (key) {
    case 'c':
      args->child_cpu = atoi(arg);
      break;
    case 'p':
      args->parent_cpu = atoi(arg);
      break;
    case 'r':
      args->repeats = atoi(arg);
      break;
    case 'i':
    case 'n':
      args->iters = atoi(arg);
      break;
    case 'w':
      args->warmup_iters = atoi(arg);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

long long elapsed_nsec(struct timespec start, struct timespec end) {
  return (end.tv_sec - start.tv_sec) * 1000000000 +
         (end.tv_nsec - start.tv_nsec);
}

static struct argp argp = {options, parse_opt, 0, 0, 0, 0, 0};

void sched_setaffinity_or_die(int cpu_id) {
  if (cpu_id == DEFAULT_CPU) {
    return;
  }

  cpu_set_t cpu_set;
  CPU_ZERO(&cpu_set);
  CPU_SET(cpu_id, &cpu_set);

  if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &cpu_set) == -1) {
    perror("could not set CPU affinity");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char **argv) {
  args args;
  state *state = new_state();

  args.child_cpu = DEFAULT_CPU;
  args.parent_cpu = DEFAULT_CPU;
  args.iters = DEFAULT_ITERS;
  args.repeats = DEFAULT_MEASUREMENTS;
  args.warmup_iters = DEFAULT_WARMUP_ITERS;

  argp_parse(&argp, argc, argv, 0, 0, &args);

  pre_fork_setup(state);

  pid_t child = fork();
  if (child == -1) {
    perror("could not fork");
    exit(EXIT_FAILURE);
  } else if (child) {
    long long *data = calloc(sizeof(long long), args.repeats);
    sched_setaffinity_or_die(args.parent_cpu);
    parent_post_fork_setup(state);
    parent_warmup(args.warmup_iters, state);

    struct timespec start, end;

    for (int i = 0; i < args.repeats; ++i) {
      clock_gettime(CLOCK_MONOTONIC, &start);
      parent_loop(args.iters, state);
      clock_gettime(CLOCK_MONOTONIC, &end);
      data[i] = elapsed_nsec(start, end);
    }

    if (args.repeats == 1) {
      long long elapsed = elapsed_nsec(start, end);
      fprintf(stderr, "%d iters in %lld ns\n %f ns/iter\n", args.iters, data[0],
              (double)elapsed / args.iters);
    } else {
      for (int i = 0; i < args.repeats; ++i) {
        printf("%d\t%lld\n", args.iters, data[i]);
      }
    }

    parent_cleanup(state);
    kill(child, SIGTERM);

  } else {
    sched_setaffinity_or_die(args.child_cpu);
    child_post_fork_setup(state);
    child_warmup(args.warmup_iters, state);
    child_loop(args.iters, state);
    child_cleanup(state);
  }
  cleanup(state);
  free_state(state);

  return 0;
}
