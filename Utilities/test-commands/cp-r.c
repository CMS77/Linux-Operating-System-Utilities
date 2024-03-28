#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <linux/limits.h>
#include <bits/getopt_core.h>

#define BUFFER_SIZE 1024

void copy_file(const char *source, const char *destination) {
    FILE *src, *dest;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    src = fopen(source, "rb");
    if (src == NULL) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    dest = fopen(destination, "wb");
    if (dest == NULL) {
        perror("Error opening destination file");
        fclose(src);
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src)) > 0) {
        fwrite(buffer, 1, bytes_read, dest);
    }

    fclose(src);
    fclose(dest);
}

void copy_directory(const char *source, const char *destination) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char src_path[PATH_MAX], dest_path[PATH_MAX];

    dir = opendir(source);
    if (dir == NULL) {
        perror("Error opening source directory");
        exit(EXIT_FAILURE);
    }

    mkdir(destination, 0755); // Create destination directory

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(src_path, sizeof(src_path), "%s/%s", source, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", destination, entry->d_name);

        if (lstat(src_path, &statbuf) == -1) {
            perror("Error getting file status");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            copy_directory(src_path, dest_path);
        } else {
            copy_file(src_path, dest_path);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s [-r] <source> <destination>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int recursive = 0;
    int opt;
    while ((opt = getopt(argc, argv, "r")) != -1) {
        switch (opt) {
            case 'r':
                recursive = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-r] <source> <destination>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const char *source = argv[optind];
    const char *destination = argv[optind + 1];

    if (recursive) {
        copy_directory(source, destination);
    } else {
        copy_file(source, destination);
    }

    return 0;
}
