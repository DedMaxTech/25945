CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic
TARGET = pipe_upper

all: $(TARGET)

$(TARGET): pipe_upper.c
	$(CC) $(CFLAGS) pipe_upper.c -o $(TARGET)

clean:
	rm -f $(TARGET)
