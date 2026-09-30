#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pid_t child;
    int status;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return EXIT_FAILURE;
    }

    child = fork();
    if (child == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (child == 0) {
        execlp("cat", "cat", argv[1], (char *)NULL);
        perror("cat");
        _exit(127);
    }

    printf("Parent: child %ld is running\n", (long)child);
    fflush(stdout);
    while (waitpid(child, &status, 0) == -1) {
        if (errno != EINTR) {
            perror("waitpid");
            return EXIT_FAILURE;
        }
    }
    puts("Parent: child has finished");
    return WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;
}
