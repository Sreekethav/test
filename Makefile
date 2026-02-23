# Simple Makefile for strparser
CC := gcc
CFLAGS := -Wall -Wextra -fPIC

SRCS := strparser.c
OBJS := $(SRCS:.c=.o)
TARGET := libstrparser.a

all: $(TARGET)

$(TARGET): $(OBJS)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
