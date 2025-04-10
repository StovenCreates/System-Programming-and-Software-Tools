#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "pish.h"

static char pish_history_path[1024] = {'\0'};

/*
 * Set history file path to ~/.pish_history.
 */
static void set_history_path()
{
    const char *home = getpwuid(getuid())->pw_dir;
    strncpy(pish_history_path, home, 1024);
    strcat(pish_history_path, "/.pish_history");
}

void add_history(const struct pish_arg *arg)
{
    FILE *file;

    if (!(*pish_history_path)) {
        set_history_path();
    }

    file = fopen(pish_history_path, "a");
    if (!file) {
        perror("open");
        return;
    }

    for (int i = 0; i < arg->argc; i++) {
        fprintf(file, "%s", arg->argv[i]);
        if (i < arg->argc - 1) {
            fprintf(file, " ");
        }
    }

    fprintf(file, "\n");
    fclose(file);
}

void print_history()
{
    FILE *file;
    char line[1024];
    int count = 1;

    if (!(*pish_history_path)) {
        set_history_path();
    }

    file = fopen(pish_history_path, "r");
    if (!file) {
        perror("open");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        printf("%d %s", count++, line); 
    }

    fclose(file);
}
