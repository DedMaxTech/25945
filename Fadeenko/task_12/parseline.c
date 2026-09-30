#include <ctype.h>
#include <string.h>
#include "shell.h"

int parseline(const char *line, struct command *cmd)
{
    const char *p = line;
    size_t used = 0;

    memset(cmd, 0, sizeof(*cmd));
    while (*p != '\0') {
        int kind = 0;
        const char *start;
        size_t length;
        char *word;

        while (isspace((unsigned char)*p))
            ++p;
        if (*p == '\0')
            break;
        if (*p == '|' || *p == ';')
            return -1;
        if (*p == '&') {
            ++p;
            while (isspace((unsigned char)*p))
                ++p;
            if (*p != '\0' || cmd->argc == 0)
                return -1;
            cmd->bkgrnd = 1;
            break;
        }
        if (*p == '<') {
            kind = 1;
            ++p;
        } else if (*p == '>') {
            kind = p[1] == '>' ? 3 : 2;
            p += kind == 3 ? 2 : 1;
        }
        while (isspace((unsigned char)*p))
            ++p;
        if (*p == '\0' || *p == '<' || *p == '>' || *p == '&' ||
            *p == '|' || *p == ';')
            return -1;

        start = p;
        while (*p != '\0' && !isspace((unsigned char)*p) &&
               *p != '<' && *p != '>' && *p != '&' && *p != '|' && *p != ';')
            ++p;
        length = (size_t)(p - start);
        if (used + length + 1 > sizeof(cmd->storage))
            return -1;
        word = cmd->storage + used;
        memcpy(word, start, length);
        word[length] = '\0';
        used += length + 1;

        if (kind == 1) {
            if (cmd->infile != NULL)
                return -1;
            cmd->infile = word;
        } else if (kind == 2 || kind == 3) {
            if (cmd->outfile != NULL || cmd->appfile != NULL)
                return -1;
            if (kind == 2)
                cmd->outfile = word;
            else
                cmd->appfile = word;
        } else {
            if (cmd->argc >= 63)
                return -1;
            cmd->cmdargs[cmd->argc++] = word;
        }
    }

    cmd->cmdargs[cmd->argc] = NULL;
    if (cmd->argc == 0 &&
        (cmd->infile != NULL || cmd->outfile != NULL || cmd->appfile != NULL))
        return -1;
    return cmd->argc != 0;
}
