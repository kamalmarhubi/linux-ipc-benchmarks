# Public domain Makefile template derived from
#   http://jingram.sdf.org/2014/11/09/a-makefile-template-for-simple-c-projects.html

PROGS := pipes eventfd
SRCS := $(addsuffix .c, $(PROGS)) bench_main.c

CC := cc
CFLAGS := -Wall -Wextra -Werror -MMD
LDFLAGS :=

$(PROGS): $(addsuffix .o, $@) bench_main.o

OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

.PHONY: all
all: $(PROGS)

-include $(DEPS)

.PHONY: clean cleaner
clean:
	rm -f $(OBJS) $(DEPS)

cleaner: clean
	rm -rf $(PROGS)
