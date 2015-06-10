# Public domain Makefile template derived from
#   http://jingram.sdf.org/2014/11/09/a-makefile-template-for-simple-c-projects.html

.DEFAULT_GOAL = all

PROGS := pipes eventfd af_unix
SRCS := $(addsuffix .c, $(PROGS)) main.c

CC := cc
CFLAGS := -std=gnu11 -Wall -Wextra -Werror -MMD
LDFLAGS :=

$(PROGS): $(addsuffix .o, $@) main.o

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
