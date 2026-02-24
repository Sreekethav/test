# Simple Makefile for strparser
# Respect OSS-Fuzz environment variables
CC ?= gcc
CFLAGS ?= -Wall -Wextra
CFLAGS += -fPIC -Iinclude

SRCS := src/strparser.c
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
