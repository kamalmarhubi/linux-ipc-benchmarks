# Public domain Makefile template from
#   http://jingram.sdf.org/2014/11/09/a-makefile-template-for-simple-c-projects.html

PROG := pipes
SRCS := pipes.c

CC      := cc
CFLAGS  := -Wall -Wextra -Werror
LDFLAGS :=

OBJS   = $(SRCS:.c=.o)
DEPS   = $(SRCS:.c=.d)

.PHONY: all
all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

-include $(DEPS)

.PHONY: clean cleaner
clean:
	rm -f $(OBJS) $(DEPS)

cleaner: clean
	rm -rf $(PROG)
