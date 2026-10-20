#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

static volatile sig_atomic_t stopped;

static void stop_editor(int number)
{
    stopped = number;
}

int main(void)
{
    struct termios saved, mode;
    struct sigaction action = {0};
    char line[40];
    size_t length = 0;
    unsigned char c;
    int result = EXIT_SUCCESS;

    if (tcgetattr(STDIN_FILENO, &saved) == -1) {
        perror("tcgetattr");
        return EXIT_FAILURE;
    }
    action.sa_handler = stop_editor;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGINT, &action, NULL) == -1 ||
        sigaction(SIGQUIT, &action, NULL) == -1 ||
        sigaction(SIGTERM, &action, NULL) == -1 ||
        sigaction(SIGHUP, &action, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    mode = saved;
    mode.c_lflag &= ~(ICANON | ECHO | ECHONL | ISIG | IEXTEN);
    mode.c_iflag &= ~(IXON | IXOFF | ICRNL | INLCR | IGNCR);
    mode.c_cc[VMIN] = 1;
    mode.c_cc[VTIME] = 0;
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("Type text. ERASE: last character, KILL: line, Ctrl-W: word.");
    puts("Ctrl-D on an empty line: exit. Line width: 40 ASCII characters.");
    if (tcsetattr(STDIN_FILENO, TCSANOW, &mode) == -1) {
        perror("tcsetattr");
        return EXIT_FAILURE;
    }

    while (!stopped) {
        ssize_t bytes = read(STDIN_FILENO, &c, 1);

        if (bytes == 0)
            break;
        if (bytes == -1) {
            if (errno == EINTR)
                continue;
            perror("read");
            result = EXIT_FAILURE;
            break;
        }
        if (c == 4 && length == 0)
            break;
        if (c == saved.c_cc[VERASE] || c == saved.c_cc[VKILL] || c == 23) {
            size_t keep = length;

            if (c == saved.c_cc[VERASE]) {
                if (keep != 0)
                    --keep;
            } else if (c == saved.c_cc[VKILL]) {
                keep = 0;
            } else {
                while (keep != 0 && line[keep - 1] == ' ')
                    --keep;
                while (keep != 0 && line[keep - 1] != ' ')
                    --keep;
            }
            while (length > keep) {
                fputs("\b \b", stdout);
                --length;
            }
        } else if (c == '\n' || c == '\r') {
            putchar('\n');
            length = 0;
        } else if (!isprint(c)) {
            putchar('\a');
        } else {
            if (length == sizeof(line)) {
                size_t start = length;

                if (c != ' ') {
                    while (start != 0 && line[start - 1] != ' ')
                        --start;
                }
                if (start == 0 || start == length) {
                    putchar('\n');
                    length = 0;
                } else {
                    size_t word_length = length - start;

                    for (size_t i = 0; i < word_length; ++i)
                        fputs("\b \b", stdout);
                    putchar('\n');
                    memmove(line, line + start, word_length);
                    fwrite(line, 1, word_length, stdout);
                    length = word_length;
                }
                if (c == ' ')
                    continue;
            }
            line[length++] = (char)c;
            putchar(c);
        }
    }

    if (tcsetattr(STDIN_FILENO, TCSANOW, &saved) == -1) {
        perror("tcsetattr");
        result = EXIT_FAILURE;
    }
    putchar('\n');
    return stopped ? 128 + stopped : result;
}
