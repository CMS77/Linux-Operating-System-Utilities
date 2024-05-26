#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>

#define BUF_SIZE 4096

void print_options() {
    printf("\nOptions:\n\n");
    printf("  <source> <dest>        : Copy files or directories.\n");
    printf("  -r <source> <dest>     : Copy directories recursively.\n");
    printf("  -p <source> <dest>     : Preserve file attributes.\n");
    printf("  -f <source> <dest>     : Force copy by overwriting destination.\n");
    printf("  -v <source> <dest>     : Verbose mode.\n");
    printf("  --help                 : Display this help message.\n\n");
}

void copy_file(const char *source, const char *dest, int preserve, int force, int verbose) {
    int src_fd, dest_fd, nread;
    char buf[BUF_SIZE];
    struct stat src_stat;
    mode_t mode;

    src_fd = open(source, O_RDONLY);
    if (src_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    fstat(src_fd, &src_stat);
    mode = preserve ? src_stat.st_mode : S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    dest_fd = open(dest, O_WRONLY | O_CREAT | (force ? O_TRUNC : O_EXCL), mode);
    if (dest_fd == -1) {
        if (!force) {
            char response;
            printf("Destination file already exists. Overwrite? (y/n): ");
            scanf(" %c", &response);
            if (response != 'y' && response != 'Y') {
                printf("Operation canceled.\n");
                exit(EXIT_SUCCESS);
            }
            dest_fd = open(dest, O_WRONLY | O_TRUNC, mode);
            if (dest_fd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
        } else {
            perror("open");
            exit(EXIT_FAILURE);
        }
    }

    while ((nread = read(src_fd, buf, BUF_SIZE)) > 0) {
        if (write(dest_fd, buf, nread) != nread) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    if (nread == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    if (verbose) {
        printf("'%s' -> '%s'\n", source, dest);
    }

    close(src_fd);
    close(dest_fd);
}

void copy_directory(const char *source, const char *dest, int preserve, int force, int verbose) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char src_path[PATH_MAX];
    char dest_path[PATH_MAX];

    if ((dir = opendir(source)) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    if (mkdir(dest, 0777) == -1) {
        perror("mkdir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(src_path, sizeof(src_path), "%s/%s", source, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->d_name);

        if (lstat(src_path, &st) == -1) {
            perror("lstat");
            exit(EXIT_FAILURE);
        }

        if (S_ISDIR(st.st_mode)) {
            copy_directory(src_path, dest_path, preserve, force, verbose);
        } else {
            copy_file(src_path, dest_path, preserve, force, verbose);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    int opt;
    int recursive = 0, preserve = 0, force = 0, verbose = 0;

    while ((opt = getopt(argc, argv, "rpfvh")) != -1) {
        switch (opt) {
            case 'r':
                recursive = 1;
                break;
            case 'p':
                preserve = 1;
                break;
            case 'f':
                force = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'h':
                print_options();
                exit(EXIT_SUCCESS);
            default:
                print_options();
                return EXIT_FAILURE;
        }
    }

    if (argc - optind != 2) {
        fprintf(stderr, "Invalid number of arguments.\n");
        return EXIT_FAILURE;
    }

    if (recursive) {
        copy_directory(argv[optind], argv[optind + 1], preserve, force, verbose);
    } else {
        copy_file(argv[optind], argv[optind + 1], preserve, force, verbose);
    }

    return EXIT_SUCCESS;
}
