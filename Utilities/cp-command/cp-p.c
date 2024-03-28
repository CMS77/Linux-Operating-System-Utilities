#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <utime.h>
#include <bits/getopt_core.h>

#define BUFFER_SIZE 1024

void copy_file(const char *source, const char *destination, int preserve_attributes) {
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

    if (preserve_attributes) {
        struct stat statbuf;
        if (stat(source, &statbuf) == -1) {
            perror("Error getting file status");
            exit(EXIT_FAILURE);
        }

        if (chmod(destination, statbuf.st_mode) == -1) {
            perror("Error setting file permissions");
            exit(EXIT_FAILURE);
        }

        struct utimbuf times;
        times.actime = statbuf.st_atime;
        times.modtime = statbuf.st_mtime;
        if (utime(destination, &times) == -1) {
            perror("Error setting file timestamps");
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s [-p] <source> <destination>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int preserve_attributes = 0;
    int opt;
    while ((opt = getopt(argc, argv, "p")) != -1) {
        switch (opt) {
            case 'p':
                preserve_attributes = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-p] <source> <destination>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const char *source = argv[optind];
    const char *destination = argv[optind + 1];

    copy_file(source, destination, preserve_attributes);

    return 0;
}


//Usage: ./cp-p -p  cp-p.c ../test-commands/cp-p.c 
