CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic
TARGET = shell
SOURCES = shell.c ../task_12/parseline.c ../task_12/promptline.c

all: $(TARGET)

$(TARGET): $(SOURCES) ../task_12/shell.h
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)
