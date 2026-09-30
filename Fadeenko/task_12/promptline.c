#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"

int promptline(char *line, size_t size)
{
    int c;

    if (isatty(STDIN_FILENO)) {
        fputs("$ ", stdout);
        fflush(stdout);
    }
    if (fgets(line, (int)size, stdin) == NULL)
        return 0;
    if (strchr(line, '\n') == NULL && !feof(stdin)) {
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        return -1;
    }
    return 1;
}
