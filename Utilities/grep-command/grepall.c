#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // for tolower
#include <dirent.h>
#include <sys/stat.h>

#define MAX_LINE_LENGTH 1024

// Function to search for a pattern within a line (case-insensitive)
void grep(const char *pattern, const char *filename, int ignore_case, int include_line_numbers) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        line_number++;
        const char *line_to_search = line;
        const char *pattern_to_match = pattern;

        if (ignore_case) {
            // Convert both line and pattern to lowercase
            char lowercase_line[MAX_LINE_LENGTH];
            char lowercase_pattern[MAX_LINE_LENGTH];
            for (int i = 0; line[i]; i++) {
                lowercase_line[i] = tolower(line[i]);
            }
            for (int i = 0; pattern[i]; i++) {
                lowercase_pattern[i] = tolower(pattern[i]);
            }
            lowercase_line[strlen(line)] = '\0';
            lowercase_pattern[strlen(pattern)] = '\0';
            line_to_search = lowercase_line;
            
            pattern_to_match = lowercase_pattern;
        }

        if (strstr(line_to_search, pattern_to_match) != NULL) {
            if (include_line_numbers) {
                printf("%d:%s", line_number, line);
            } else {
                printf("%s", line);
            }
        }
    }
    
    fclose(file);
}

// Function to recursively search directories for files matching the pattern
void grep_recursive(const char *pattern, const char *dir_name, int ignore_case, int include_line_numbers) {
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
            grep_recursive(pattern, path, ignore_case, include_line_numbers); // Recursive call for directories
        } else {
            grep(pattern, path, ignore_case, include_line_numbers); // Search the file for the pattern
        }
    }

    closedir(dir);
}

// Function to search for lines that do not contain a pattern
void grep_v(const char *pattern, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (strstr(line, pattern) == NULL) {
            printf("%s", line);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s [-r] [-i] [-n] [-v] pattern file1 [file2 ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int ignore_case = 0;
    int include_line_numbers = 0;
    int invert_match = 0;
    int arg_index = 1;

    // Check for flags and adjust argument index accordingly
    while (argv[arg_index][0] == '-') {
        if (strcmp(argv[arg_index], "-i") == 0) {
            ignore_case = 1;
        } else if (strcmp(argv[arg_index], "-n") == 0) {
            include_line_numbers = 1;
        } else if (strcmp(argv[arg_index], "-r") == 0) {
            arg_index++;
            const char *pattern = argv[arg_index];
            for (int i = arg_index + 1; i < argc; i++) {
                grep_recursive(pattern, argv[i], ignore_case, include_line_numbers);
            }
            return EXIT_SUCCESS;
        } else if (strcmp(argv[arg_index], "-v") == 0) {
            invert_match = 1;
            arg_index = 2;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[arg_index]);
            return EXIT_FAILURE;
        }
        arg_index++;
    }

    const char *pattern = argv[arg_index];

    // Call appropriate function based on -v option
    if (invert_match) {
        for (int i = arg_index + 1; i < argc; i++) {
            grep_v(pattern, argv[i]);
        }
    } else {
        for (int i = arg_index + 1; i < argc; i++) {
            grep(pattern, argv[i]);
        }
    }

    return EXIT_SUCCESS;
}
