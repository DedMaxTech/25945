#include <grp.h>
#include <inttypes.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int result = EXIT_SUCCESS;
    const mode_t bits[] = {
        S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP,
        S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH
    };

    if (argc < 2) {
        fprintf(stderr, "Usage: %s path [path...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; ++i) {
        struct stat info;
        struct passwd *owner;
        struct group *group;
        struct tm modified;
        char permissions[11] = "----------";
        char owner_id[32], group_id[32], size[32] = "";
        char date[32] = "?";
        char *path, *name;
        size_t length;

        if (lstat(argv[i], &info) == -1) {
            perror(argv[i]);
            result = EXIT_FAILURE;
            continue;
        }
        path = strdup(argv[i]);
        if (path == NULL) {
            perror("strdup");
            return EXIT_FAILURE;
        }
        length = strlen(path);
        while (length > 1 && path[length - 1] == '/')
            path[--length] = '\0';
        name = strrchr(path, '/');
        name = name == NULL ? path : name[1] == '\0' ? name : name + 1;

        permissions[0] = S_ISDIR(info.st_mode) ? 'd' : S_ISREG(info.st_mode) ? '-' : '?';
        for (size_t j = 0; j < 9; ++j)
            if (info.st_mode & bits[j])
                permissions[j + 1] = "rwx"[j % 3];
        snprintf(owner_id, sizeof(owner_id), "%ju", (uintmax_t)info.st_uid);
        snprintf(group_id, sizeof(group_id), "%ju", (uintmax_t)info.st_gid);
        owner = getpwuid(info.st_uid);
        group = getgrgid(info.st_gid);
        if (S_ISREG(info.st_mode))
            snprintf(size, sizeof(size), "%jd", (intmax_t)info.st_size);
        if (localtime_r(&info.st_mtime, &modified) != NULL)
            strftime(date, sizeof(date), "%Y-%m-%d %H:%M", &modified);

        printf("%s %4ju %-12s %-12s %10s %s %s\n", permissions,
               (uintmax_t)info.st_nlink, owner == NULL ? owner_id : owner->pw_name,
               group == NULL ? group_id : group->gr_name, size, date, name);
        free(path);
    }
    return result;
}
