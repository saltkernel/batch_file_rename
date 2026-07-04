#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/stat.h>

void list_files(const char *dirpath) {
    DIR *dir = opendir(dirpath);
    if (!dir) {
        fprintf(stderr, "Cannot open directory: %s\n", dirpath);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        size_t len = strlen(dirpath) + strlen(entry->d_name) + 2;
        char *fullpath = malloc(len);
        if (!fullpath) {
            fprintf(stderr, "Memory allocation failed\n");
            closedir(dir);
            return;
        }
        snprintf(fullpath, len, "%s/%s", dirpath, entry->d_name);

        struct stat st;
        if (stat(fullpath, &st) != 0) {
            fprintf(stderr, "Cannot stat: %s\n", fullpath);
            free(fullpath);
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            list_files(fullpath);
        } else if (S_ISREG(st.st_mode)) {
            printf("%s\n", fullpath);
        }

        free(fullpath);
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    char *match = NULL;
    char *replace = NULL;
    char *dir = NULL;
    int opt;

    static struct option long_options[] = {
        {"match", required_argument, NULL, 'm'},
        {"replace", required_argument, NULL, 'r'},
        {NULL, 0, NULL, 0}
    };

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <directory> --match <pattern> --replace <replacement>\n", argv[0]);
        return 1;
    }

    while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (opt) {
            case 'm':
                match = optarg;
                break;
            case 'r':
                replace = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s <directory> --match <pattern> --replace <replacement>\n", argv[0]);
                return 1;
        }
    }

    if (optind < argc) {
        dir = argv[optind];
    }

    if (!match || !replace || !dir) {
        fprintf(stderr, "Usage: %s <directory> --match <pattern> --replace <replacement>\n", argv[0]);
        return 1;
    }

    list_files(dir);

    return 0;
}
