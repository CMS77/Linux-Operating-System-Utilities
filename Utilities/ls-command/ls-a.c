#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char *argv[]) {
    DIR *directory;
    struct dirent *entry;
    struct stat statbuf;

    // Open current directory if no directory provided as argument
    if (argc < 2 || strcmp(argv[1], "-a") == 0) {
        directory = opendir(".");
    } else {
        directory = opendir(argv[1]);
    }

    // Check if directory opened successfully
    if (directory == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    // Read directory entries
    while ((entry = readdir(directory)) != NULL) {
        // Get file information
        if (fstatat(dirfd(directory), entry->d_name, &statbuf, 0) == -1) {
            perror("Error getting file status");
            continue;
        }

        // Print file name
        printf("%s\n", entry->d_name);
    }

    // Close directory
    closedir(directory);
    
    return 0;
}
