#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

void print_options() {
    printf("\nOptions:\n\n");
    printf(" <pattern> <file>...         : Search pattern\n");
    printf("  -i <pattern> <file>...     : Ignore Uppercase/Lowercase\n");
    printf("  -n <pattern> <file>...     : Output lines\n");
    printf("  -r <pattern> <path>...     : Recursive search\n");
    printf("  -v <pattern> <file>...     : Invert search\n");
    printf("  --help                     : Display this help message\n\n");
}

void print_file_matches(const char *filename, const char *pattern, int ignore_upper_lower_case, int show_line, int invert_search) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_number = 0;
    while ((read = getline(&line, &len, file)) != -1) {
        line_number++;
        char *match;
        if (ignore_upper_lower_case) {
            match = strcasestr(line, pattern);
        } else {
            match = strstr(line, pattern);
        }

        if ((match && !invert_search) || (!match && invert_search)) {
            if (show_line) {
                printf("%s : %d : %s", filename, line_number, line);
            } else {
                printf("%s : %s", filename, line);
            }
        }
    }

    free(line);
    fclose(file);
}

void grep_directory(const char *dir_path, const char *pattern, int ignore_upper_lower_case, int show_line, int invert_search) {
    DIR *directory = opendir(dir_path);
    if (directory == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    struct stat statbuf;
    while ((entry = readdir(directory)) != NULL) {
        char filepath[PATH_MAX];
        snprintf(filepath, PATH_MAX, "%s/%s", dir_path, entry->d_name);
        if (lstat(filepath, &statbuf) == -1) {
            perror("Error getting file status");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                grep_directory(filepath, pattern, ignore_upper_lower_case, show_line, invert_search);
            }
        } else {
            print_file_matches(filepath, pattern, ignore_upper_lower_case, show_line, invert_search);
        }
    }

    closedir(directory);
}

int main(int argc, char *argv[]) {
    int ignore_upper_lower_case = 0; // -i
    int show_line = 0; // -n
    int recursive_search = 0; // -r
    int invert_search = 0; // -v
    const char *pattern = NULL;
    int path_encountered = 0;

    for (int i = 1; i < argc; i++) {
        if (!path_encountered && argv[i][0] == '-') {
            if (strcmp(argv[i], "-i") == 0) {
                ignore_upper_lower_case = 1;
            } else if (strcmp(argv[i], "-n") == 0) {
                show_line = 1;
            } else if (strcmp(argv[i], "-r") == 0) {
                recursive_search = 1;
            } else if (strcmp(argv[i], "-v") == 0) {
                invert_search = 1;
            } else if (strcmp(argv[i], "--help") == 0) {
                print_options();
                return EXIT_SUCCESS;
            } else {
                printf("Invalid option: %s. Check --help\n", argv[i]);
                return EXIT_FAILURE;
            }
        } else {
            if (pattern == NULL) {
                pattern = argv[i];
            } else {
                path_encountered = 1;
                for (int j = i; j < argc; j++) {
                    print_file_matches(argv[j], pattern, ignore_upper_lower_case, show_line, invert_search);
                }
                if (recursive_search) {
                    for (int j = i; j < argc; j++) {
                        grep_directory(argv[j], pattern, ignore_upper_lower_case, show_line, invert_search);
                    }
                }
                break;
            }
        }
    }

    if (pattern == NULL || argc < 2) {
        printf("Invalid. Check --help.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
