#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_LINE_LENGTH 1024

// Function to search for a pattern within a line
void grep(const char *pattern, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (strstr(line, pattern) != NULL) {
            printf("%s", line);
        }
    }

    fclose(file);
}

// Function to recursively search directories for files matching the pattern
void grep_recursive(const char *pattern, const char *dir_name) {
    DIR *dir = opendir(dir_name);
    if (dir == NULL) {
        fprintf(stderr, "Error opening directory: %s\n", dir_name);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue; // Skip . and ..
        }

        char path[MAX_LINE_LENGTH];
        snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);

        struct stat statbuf;
        if (stat(path, &statbuf) == -1) {
            perror("Error getting file status");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            grep_recursive(pattern, path); // Recursive call for directories
        } else {
            grep(pattern, path); // Search the file for the pattern
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 3 || strcmp(argv[1], "-r") != 0) {
        fprintf(stderr, "Usage: %s -r pattern directory\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *pattern = argv[2];
    const char *directory = argv[3];

    grep_recursive(pattern, directory);

    return EXIT_SUCCESS;
}
