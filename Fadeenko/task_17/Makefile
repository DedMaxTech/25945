CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic
TARGET = line_editor

all: $(TARGET)

$(TARGET): line_editor.c
	$(CC) $(CFLAGS) line_editor.c -o $(TARGET)

clean:
	rm -f $(TARGET)
