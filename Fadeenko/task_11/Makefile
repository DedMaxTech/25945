CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic
TARGET = execvpe

all: $(TARGET)

$(TARGET): execvpe.c
	$(CC) $(CFLAGS) execvpe.c -o $(TARGET)

clean:
	rm -f $(TARGET)
