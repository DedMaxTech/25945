#ifndef SHELL_H
#define SHELL_H

#include <stddef.h>

struct command {
    char *cmdargs[64];
    size_t argc;
    char *infile;
    char *outfile;
    char *appfile;
    int bkgrnd;
    char storage[1024];
};

int promptline(char *line, size_t size);
int parseline(const char *line, struct command *cmd);

#endif
