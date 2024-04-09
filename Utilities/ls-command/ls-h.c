#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h> 

// Function to convert file size to a human-readable format
void human_readable_size(off_t size, char *buffer, size_t buffer_size) {
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;

    while (size > 1024 && unit_index < 4) {
        size /= 1024;
        unit_index++;
    }

    snprintf(buffer, buffer_size, "%ld %s", (long)size, units[unit_index]);
}

int main(int argc, char *argv[]) {
    DIR *directory;
    struct dirent *entry;
    struct stat statbuf;

    // Check if -h argument is provided
    int human_readable = 0;
    if (argc >= 2 && strcmp(argv[1], "-h") == 0) {
        human_readable = 1;
    }

    // Open current directory if no directory provided as argument
    if (argc < 2 || (argc >= 2 && human_readable)) {
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
        printf("%s", entry->d_name);

        // Print file size in human-readable format if -h argument is provided
        if (human_readable) {
            char size_buffer[20];
            human_readable_size(statbuf.st_size, size_buffer, sizeof(size_buffer));
            printf(" (%s)", size_buffer);
        }

        printf("\n");
    }

    // Close directory
    closedir(directory);
    
    return 0;
}
