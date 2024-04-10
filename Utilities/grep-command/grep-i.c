#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // for tolower

#define MAX_LINE_LENGTH 1024

// Function to search for a pattern within a line (case-insensitive)
void grep(const char *pattern, const char *filename, int ignore_case) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
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
    int arg_index = 1;

    // Check if the first argument is the -i option
    if (strcmp(argv[1], "-i") == 0) {
        ignore_case = 1;
        arg_index = 2;
    }

    const char *pattern = argv[arg_index];

    for (int i = arg_index + 1; i < argc; i++) {
        grep(pattern, argv[i], ignore_case);
    }

    return EXIT_SUCCESS;
}
