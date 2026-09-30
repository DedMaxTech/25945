#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

static int try_exec(const char *path, char *const argv[], char *const envp[])
{
    size_t count = 0;
    char **shell_args;
    int error;

    execve(path, argv, envp);
    if (errno != ENOEXEC)
        return -1;

    while (argv[count] != NULL)
        ++count;
    shell_args = calloc(count + 2, sizeof(*shell_args));
    if (shell_args == NULL)
        return -1;
    shell_args[0] = "sh";
    shell_args[1] = (char *)path;
    for (size_t i = 1; i < count; ++i)
        shell_args[i + 1] = argv[i];
    execve("/bin/sh", shell_args, envp);
    error = errno;
    free(shell_args);
    errno = error;
    return -1;
}

int execvpe(const char *file, char *const argv[], char *const envp[])
{
    const char *path;
    const char *part;
    int denied = 0;

    if (*file == '\0') {
        errno = ENOENT;
        return -1;
    }
    if (strchr(file, '/') != NULL)
        return try_exec(file, argv, envp);

    path = getenv("PATH");
    if (path == NULL)
        path = "/usr/bin:/bin";
    part = path;

    for (;;) {
        const char *end = strchr(part, ':');
        size_t length = end == NULL ? strlen(part) : (size_t)(end - part);
        size_t file_length = strlen(file);
        char *candidate = malloc(length + file_length + 2);
        int error;

        if (candidate == NULL)
            return -1;
        if (length != 0) {
            memcpy(candidate, part, length);
            candidate[length] = '/';
        }
        memcpy(candidate + (length != 0 ? length + 1 : 0), file, file_length + 1);
        try_exec(candidate, argv, envp);
        error = errno;
        free(candidate);

        if (error == EACCES)
            denied = 1;
        else if (error != ENOENT && error != ENOTDIR) {
            errno = error;
            return -1;
        }
        if (end == NULL)
            break;
        part = end + 1;
    }

    errno = denied ? EACCES : ENOENT;
    return -1;
}

int main(int argc, char *argv[])
{
    char **envp;
    size_t name_length;
    size_t count = 0;
    size_t used = 0;

    if (argc < 3 || strchr(argv[1], '=') == NULL || argv[1][0] == '=') {
        fprintf(stderr, "Usage: %s NAME=value command [args...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    name_length = (size_t)(strchr(argv[1], '=') - argv[1]);
    while (environ[count] != NULL)
        ++count;
    envp = malloc((count + 2) * sizeof(*envp));
    if (envp == NULL) {
        perror("malloc");
        return EXIT_FAILURE;
    }
    for (size_t i = 0; i < count; ++i)
        if (strncmp(environ[i], argv[1], name_length) != 0 ||
            environ[i][name_length] != '=')
            envp[used++] = environ[i];
    envp[used++] = argv[1];
    envp[used] = NULL;

    execvpe(argv[2], argv + 2, envp);
    perror(argv[2]);
    free(envp);
    return 127;
}
