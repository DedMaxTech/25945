#include <stdio.h>
#include <string.h>
#include "shell.h"

int main(void)
{
    char line[1024];
    struct command cmd;
    int result;

    while ((result = promptline(line, sizeof(line))) != 0) {
        if (result == -1 || (result = parseline(line, &cmd)) == -1) {
            fputs("Invalid command line\n", stderr);
            continue;
        }
        if (result == 0)
            continue;
        if (strcmp(cmd.cmdargs[0], "exit") == 0)
            break;

        printf("Command: %s\n", cmd.cmdargs[0]);
        for (size_t i = 1; i < cmd.argc; ++i)
            printf("Argument %zu: %s\n", i, cmd.cmdargs[i]);
        if (cmd.infile != NULL)
            printf("Input: %s\n", cmd.infile);
        if (cmd.outfile != NULL)
            printf("Output: %s\n", cmd.outfile);
        if (cmd.appfile != NULL)
            printf("Append: %s\n", cmd.appfile);
        if (cmd.bkgrnd)
            puts("Background: yes");
    }
    return 0;
}
