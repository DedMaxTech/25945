#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../task_12/shell.h"

int main(void)
{
    char line[1024];
    struct command cmd;
    int result;
    int status;
    pid_t child;

    for (;;) {
        while (waitpid(-1, NULL, WNOHANG) > 0)
            ;
        result = promptline(line, sizeof(line));
        if (result == 0)
            break;
        if (result == -1 || (result = parseline(line, &cmd)) == -1) {
            fputs("Invalid command line\n", stderr);
            continue;
        }
        if (result == 0)
            continue;
        if (strcmp(cmd.cmdargs[0], "exit") == 0)
            break;
        if (cmd.infile || cmd.outfile || cmd.appfile) {
            fputs("Redirection is not available at this stage\n", stderr);
            continue;
        }

        child = fork();
        if (child == -1) {
            perror("fork");
            continue;
        }
        if (child == 0) {
            execvp(cmd.cmdargs[0], cmd.cmdargs);
            perror(cmd.cmdargs[0]);
            _exit(127);
        }
        if (cmd.bkgrnd) {
            printf("Background PID: %ld\n", (long)child);
            fflush(stdout);
        } else {
            while (waitpid(child, &status, 0) == -1) {
                if (errno != EINTR) {
                    perror("waitpid");
                    break;
                }
            }
        }
    }
    return 0;
}
