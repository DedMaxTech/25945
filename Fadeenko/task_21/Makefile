CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic
TARGET = beeper

all: $(TARGET)

$(TARGET): beeper.c
	$(CC) $(CFLAGS) beeper.c -o $(TARGET)

clean:
	rm -f $(TARGET)
