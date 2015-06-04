# Public domain Makefile template derived from
#   http://jingram.sdf.org/2014/11/09/a-makefile-template-for-simple-c-projects.html

PROGS := pipes
SRCS := $(addsuffix .c, $(PROGS))

CC      := cc
CFLAGS  := -Wall -Wextra -Werror
LDFLAGS :=

OBJS   = $(SRCS:.c=.o)

.PHONY: all
all: $(PROGS)

.PHONY: clean cleaner
clean:
	rm -f $(OBJS)

cleaner: clean
	rm -rf $(PROGS)
