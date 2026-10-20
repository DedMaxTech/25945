#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static int write_all(int fd, const char *buffer, size_t length)
{
    while (length != 0) {
        ssize_t bytes = write(fd, buffer, length);

        if (bytes == -1 && errno == EINTR)
            continue;
        if (bytes <= 0)
            return -1;
        buffer += bytes;
        length -= (size_t)bytes;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int channel[2], input, status;
    int result = EXIT_SUCCESS;
    pid_t child;
    char buffer[4096];
    ssize_t bytes;
    const char *name = argc == 2 ? argv[1] : "input.txt";

    if (argc > 2) {
        fprintf(stderr, "Usage: %s [file]\n", argv[0]);
        return EXIT_FAILURE;
    }
    input = open(name, O_RDONLY);
    if (input == -1) {
        perror(name);
        return EXIT_FAILURE;
    }
    if (pipe(channel) == -1) {
        perror("pipe");
        close(input);
        return EXIT_FAILURE;
    }
    child = fork();
    if (child == -1) {
        perror("fork");
        close(input);
        close(channel[0]);
        close(channel[1]);
        return EXIT_FAILURE;
    }
    if (child == 0) {
        close(input);
        close(channel[1]);
        for (;;) {
            bytes = read(channel[0], buffer, sizeof(buffer));
            if (bytes == -1 && errno == EINTR)
                continue;
            if (bytes <= 0)
                break;
            for (ssize_t i = 0; i < bytes; ++i)
                buffer[i] = (char)toupper((unsigned char)buffer[i]);
            if (write_all(STDOUT_FILENO, buffer, (size_t)bytes) == -1) {
                perror("write stdout");
                close(channel[0]);
                _exit(EXIT_FAILURE);
            }
        }
        if (bytes == -1)
            perror("read pipe");
        close(channel[0]);
        _exit(bytes == -1 ? EXIT_FAILURE : EXIT_SUCCESS);
    }

    close(channel[0]);
    for (;;) {
        bytes = read(input, buffer, sizeof(buffer));
        if (bytes == -1 && errno == EINTR)
            continue;
        if (bytes <= 0)
            break;
        if (write_all(channel[1], buffer, (size_t)bytes) == -1) {
            perror("write pipe");
            result = EXIT_FAILURE;
            break;
        }
    }
    if (bytes == -1) {
        perror("read file");
        result = EXIT_FAILURE;
    }
    close(input);
    close(channel[1]);
    while (waitpid(child, &status, 0) == -1) {
        if (errno != EINTR) {
            perror("waitpid");
            return EXIT_FAILURE;
        }
    }
    return result == EXIT_SUCCESS && WIFEXITED(status) && WEXITSTATUS(status) == 0
        ? EXIT_SUCCESS : EXIT_FAILURE;
}
