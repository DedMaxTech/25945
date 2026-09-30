CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic
TARGET = shell

all: $(TARGET)

$(TARGET): shell.c parseline.c promptline.c shell.h
	$(CC) $(CFLAGS) shell.c parseline.c promptline.c -o $(TARGET)

clean:
	rm -f $(TARGET)
