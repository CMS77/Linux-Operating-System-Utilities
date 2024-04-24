#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // for tolower

#define MAX_LINE_LENGTH 1024

// Function to search for a pattern within a line (case-insensitive)
void grep_i(const char *pattern, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
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

        if (strstr(lowercase_line, lowercase_pattern) != NULL) {
            printf("%s", line);
        }
    }

    fclose(file);
}

// Function to search for a pattern within a line (case-sensitive)
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

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s [-i] pattern file1 [file2 ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int ignore_case = 0;
    int pattern_index = 1;
    
    // Check if -i option is provided
    if (strcmp(argv[1], "-i") == 0) {
        ignore_case = 1;
        pattern_index = 2;
    }

    const char *pattern = argv[pattern_index];

    // Call appropriate function based on -i option
    if (ignore_case) {
        for (int i = pattern_index + 1; i < argc; i++) {
            grep_i(pattern, argv[i]);
        }
    } else {
        for (int i = pattern_index + 1; i < argc; i++) {
            grep(pattern, argv[i]);
        }
    }

    return EXIT_SUCCESS;
}
