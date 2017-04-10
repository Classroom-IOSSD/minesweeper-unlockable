SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=%.o)
CFLAGS = -Wall
TARGET = minesweeper
CC = gcc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OUTPUT_OPTION) $(CFLAGS) $^

.PHONY: clean
clean:
	$(RM) $(OBJS) $(TARGET)

