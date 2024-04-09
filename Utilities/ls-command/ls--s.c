#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

// Structure to hold file information
typedef struct {
    char *name;
    off_t size;
} FileInfo;

// Function to compare file sizes for sorting
int compare_file_sizes(const void *a, const void *b) {
    const FileInfo *fileA = (const FileInfo *)a;
    const FileInfo *fileB = (const FileInfo *)b;

    if (fileA->size < fileB->size) {
        return -1;
    } else if (fileA->size > fileB->size) {
        return 1;
    } else {
        return 0;
    }
}

int main(int argc, char *argv[]) {
    DIR *directory;
    struct dirent *entry;
    struct stat statbuf;

    int sort_by_size = 0;

    // Check command-line arguments
    if (argc >= 2 && (strcmp(argv[1], "--s") == 0 )) {
        sort_by_size = 1;
    }

    // Open current directory if no directory provided as argument
    if (argc < 2 || sort_by_size) {
        directory = opendir(".");
    } else {
        directory = opendir(argv[1]);
    }

    // Check if directory opened successfully
    if (directory == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for an array of FileInfo structures
    FileInfo *file_infos = malloc(sizeof(FileInfo) * 1000); // Assuming maximum of 1000 files
    if (file_infos == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    int num_files = 0;

    // Read directory entries
    while ((entry = readdir(directory)) != NULL) {
        // Get file information
        if (fstatat(dirfd(directory), entry->d_name, &statbuf, 0) == -1) {
            perror("Error getting file status");
            continue;
        }

        // Store file name and size in the file_infos array
        file_infos[num_files].name = strdup(entry->d_name); // strdup() to duplicate the string
        file_infos[num_files].size = statbuf.st_size;
        num_files++;
    }

    // Sort files by size if sort_by_size flag is set
    if (sort_by_size) {
        qsort(file_infos, num_files, sizeof(FileInfo), compare_file_sizes);
    }

    // Print sorted files with sizes
    for (int i = 0; i < num_files; i++) {
        printf("%-20s %lld bytes\n", file_infos[i].name, (long long)file_infos[i].size);
        free(file_infos[i].name); // Free each duplicated string
    }

    // Free allocated memory
    free(file_infos);

    // Close directory
    closedir(directory);
    
    return 0;
}
