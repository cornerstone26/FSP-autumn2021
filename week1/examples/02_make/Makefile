APP=example
SRCS=$(shell ls *.c)
OBJS=$(SRCS:.c=.o)

CC=gcc
CFLAGS=-Wall -Wextra
LDFLAGS=-lm

ifndef VERBOSE
.SILENT:
endif

.PHONY: all clean

all: $(APP)
.c.o:
	$(CC) $(CFLAGS) -c $<
$(APP): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
clean:
	rm -rf *.o $(APP)
