CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic
TARGET = command_status

all: $(TARGET)

$(TARGET): command_status.c
	$(CC) $(CFLAGS) command_status.c -o $(TARGET)

clean:
	rm -f $(TARGET)
