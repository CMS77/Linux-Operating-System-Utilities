#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        fprintf(stderr, "Usage: %s [-v] pattern file1 [file2 ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int invert_match = 0;
    int pattern_index = 1;
    
    // Check if -v option is provided
    if (strcmp(argv[1], "-v") == 0) {
        invert_match = 1;
        pattern_index = 2;
    }

    const char *pattern = argv[pattern_index];

    // Call appropriate function based on -v option
    if (invert_match) {
        for (int i = pattern_index + 1; i < argc; i++) {
            grep_v(pattern, argv[i]);
        }
    } else {
        for (int i = pattern_index + 1; i < argc; i++) {
            grep(pattern, argv[i]);
        }
    }

    return EXIT_SUCCESS;
}
