#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/stat.h>

#ifndef __FNMATCH_H__
#define __FNMATCH_H__

#define FNM_NOESCAPE 0x01
#define FNM_PATHNAME 0x02
#define FNM_PERIOD   0x04
#define FNM_CASEFOLD 0x08
#define FNM_NOMATCH  1

int fnmatch(const char *pattern, const char *string, int flags) {
    if (!pattern || !string) return 1;

    while (*pattern && *string) {
        if (*pattern == '*') {
            pattern++;
            while (*pattern == '*') pattern++;
            if (!*pattern) return 0;
            while (*string) {
                if (fnmatch(pattern, string, flags) == 0) return 0;
                string++;
            }
            return 1;
        } else if (*pattern == '?') {
            pattern++;
            string++;
        } else if (*pattern == '[' && (flags & FNM_PATHNAME) == 0) {
            pattern++;
            int match = 0;
            int negate = 0;
            if (*pattern == '!') { negate = 1; pattern++; }
            while (*pattern && *pattern != ']') {
                if (*(pattern + 1) == '-' && *(pattern + 2)) {
                    if (*string >= *pattern && *string <= *(pattern + 2)) match = 1;
                    pattern += 2;
                } else if (*pattern == *string) {
                    match = 1;
                }
                pattern++;
            }
            if (*pattern == ']') pattern++;
            if (negate) match = !match;
            if (!match) return 1;
            string++;
        } else {
            if (*pattern == *string) {
                pattern++;
                string++;
            } else if ((flags & FNM_CASEFOLD) && (*pattern | 0x20) == (*string | 0x20)) {
                pattern++;
                string++;
            } else {
                return 1;
            }
        }
    }

    while (*pattern == '*') pattern++;
    return *pattern || *string ? FNM_NOMATCH : 0;
}

#endif

char *apply_replace(const char *dirpath, const char *filename, const char *match, const char *replace) {
    size_t mlen = strlen(match);
    size_t rlen = strlen(replace);
    size_t flen = strlen(filename);
    size_t dlen = strlen(dirpath);
    size_t count = 0;
    const char *p = filename;

    while ((p = strstr(p, match)) != NULL) {
        count++;
        p += mlen;
    }

    size_t new_flen = flen + count * (rlen - mlen);
    size_t len = dlen + 1 + new_flen + 1;
    char *newpath = malloc(len);
    if (!newpath) return NULL;

    memcpy(newpath, dirpath, dlen);
    newpath[dlen] = '/';

    const char *s = filename;
    char *d = newpath + dlen + 1;
    while ((p = strstr(s, match)) != NULL) {
        size_t n = p - s;
        memcpy(d, s, n);
        d += n;
        memcpy(d, replace, rlen);
        d += rlen;
        s = p + mlen;
    }
    strcpy(d, s);

    return newpath;
}

void list_files(const char *dirpath, const char *match, const char *replace) {
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
            list_files(fullpath, match, replace);
        } else if (S_ISREG(st.st_mode)) {
            if (fnmatch(match, entry->d_name, 0) == 0) {
                char *newpath = apply_replace(dirpath, entry->d_name, match, replace);
                if (newpath) {
                    struct stat target_st;
                    if (stat(newpath, &target_st) == 0) {
                        fprintf(stderr, "[ERR] %s -> %s: target exists\n", fullpath, newpath);
                    } else if (rename(fullpath, newpath) == 0) {
                        printf("[OK] %s -> %s\n", fullpath, newpath);
                    } else {
                        fprintf(stderr, "[ERR] %s -> %s: %s\n", fullpath, newpath, strerror(errno));
                    }
                    free(newpath);
                }
            }
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
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory> --match <pattern> --replace <replacement>\n", argv[0]);
        return 1;
    }

    while ((opt = getopt_long(argc, argv, "m:r:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'm':
                match = optarg;
                break;
            case 'r':
                replace = optarg;
                break;
            case 'h':
                fprintf(stderr, "Usage: %s <directory> --match <pattern> --replace <replacement>\n", argv[0]);
                return 0;
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

    list_files(dir, match, replace);

    return 0;
}
