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

    while ((result = promptline(line, sizeof(line))) != 0) {
        if (result == -1 || (result = parseline(line, &cmd)) == -1) {
            fputs("Invalid command line\n", stderr);
            continue;
        }
        if (result == 0)
            continue;
        if (strcmp(cmd.cmdargs[0], "exit") == 0)
            break;
        if (cmd.bkgrnd || cmd.infile || cmd.outfile || cmd.appfile) {
            fputs("This stage supports simple commands only\n", stderr);
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
        while (waitpid(child, &status, 0) == -1) {
            if (errno != EINTR) {
                perror("waitpid");
                break;
            }
        }
    }
    return 0;
}
