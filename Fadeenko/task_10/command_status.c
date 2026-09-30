#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pid_t child;
    int status;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    child = fork();
    if (child == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (child == 0) {
        execvp(argv[1], argv + 1);
        perror(argv[1]);
        _exit(127);
    }

    while (waitpid(child, &status, 0) == -1) {
        if (errno != EINTR) {
            perror("waitpid");
            return EXIT_FAILURE;
        }
    }

    if (WIFEXITED(status)) {
        printf("Exit code: %d\n", WEXITSTATUS(status));
        return EXIT_SUCCESS;
    }
    if (WIFSIGNALED(status))
        printf("Terminated by signal: %d\n", WTERMSIG(status));
    return EXIT_FAILURE;
}
