#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

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

    printf("Directory: %s\n", dir);
    printf("Match:     %s\n", match);
    printf("Replace:   %s\n", replace);

    return 0;
}
