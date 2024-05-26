#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

void print_help() {
    printf("Usage: rm [OPTION]... [FILE]...\n");
    printf("Remove (unlink) the FILE(s).\n");
    printf("\nOptions:\n");
    printf("  -f        ignore nonexistent files and arguments, never prompt\n");
    printf("  -r        remove directories and their contents recursively\n");
    printf("  -v        explain what is being done\n");
    printf("  -i        prompt before every removal\n");
    printf("  --help    display this help and exit\n");
}

int remove_file(const char *path, int force, int verbose, int interactive) {
    if (interactive) {
        printf("rm: remove '%s'? ", path);
        int c = getchar();
        if (c != 'y' && c != 'Y') {
            printf("not removed\n");
            return 0;
        }
        while (c != '\n' && c != EOF) c = getchar(); // Consume the newline
    }
    if (unlink(path) == 0) {
        if (verbose) printf("removed '%s'\n", path);
        return 0;
    } else {
        if (force && errno == ENOENT) {
            return 0; // Ignore nonexistent files
        }
        perror("rm");
        return -1;
    }
}

int remove_directory(const char *path, int force, int verbose, int interactive) {
    DIR *dir = opendir(path);
    if (!dir) {
        if (force && errno == ENOENT) {
            return 0; // Ignore nonexistent directories
        }
        perror("rm");
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                remove_directory(full_path, force, verbose, interactive);
            } else {
                remove_file(full_path, force, verbose, interactive);
            }
        } else {
            perror("rm");
        }
    }

    closedir(dir);

    if (interactive) {
        printf("rm: remove directory '%s'? ", path);
        int c = getchar();
        if (c != 'y' && c != 'Y') {
            printf("not removed\n");
            return 0;
        }
        while (c != '\n' && c != EOF) c = getchar(); // Consume the newline
    }

    if (rmdir(path) == 0) {
        if (verbose) printf("removed directory '%s'\n", path);
        return 0;
    } else {
        perror("rm");
        return -1;
    }
}

int main(int argc, char *argv[]) {
    int force = 0, recursive = 0, verbose = 0, interactive = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        } else if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'f':
                        force = 1;
                        break;
                    case 'r':
                        recursive = 1;
                        break;
                    case 'v':
                        verbose = 1;
                        break;
                    case 'i':
                        interactive = 1;
                        break;
                    default:
                        fprintf(stderr, "rm: invalid option -- '%c'\n", argv[i][j]);
                        print_help();
                        return 1;
                }
            }
        } else {
            struct stat st;
            if (stat(argv[i], &st) == 0) {
                if (S_ISDIR(st.st_mode)) {
                    if (recursive) {
                        remove_directory(argv[i], force, verbose, interactive);
                    } else {
                        fprintf(stderr, "rm: cannot remove '%s': Is a directory\n", argv[i]);
                        return 1;
                    }
                } else {
                    remove_file(argv[i], force, verbose, interactive);
                }
            } else {
                if (force) continue; // Ignore nonexistent files
                perror("rm");
                return 1;
            }
        }
    }

    return 0;
}
