# Compiler
CC = gcc

# Compiler flags (warnings + debug info)
CFLAGS = -Wall -Wextra -g -Iinclude

# Executable name
TARGET = mysh

# Source files
SRCS = main.c $(wildcard src/*.c)

# Object files
OBJS = $(SRCS:.c=.o)

# Default target to build executable
all: $(TARGET)

# Link object files into executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile .c into .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

