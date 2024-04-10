#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

// Function to search for a pattern within a line
void grep(const char *pattern, const char *filename, int include_line_numbers) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        line_number++;
        if (strstr(line, pattern) != NULL) {
            if (include_line_numbers) {
                printf("%d:%s", line_number, line);
            } else {
                printf("%s", line);
            }
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s [-n] pattern file1 [file2 ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int include_line_numbers = 0;
    int arg_index = 1;

    // Check if the first argument is the -n flag
    if (strcmp(argv[1], "-n") == 0) {
        include_line_numbers = 1;
        arg_index++;
    }

    const char *pattern = argv[arg_index];

    for (int i = arg_index + 1; i < argc; i++) {
        grep(pattern, argv[i], include_line_numbers);
    }

    return EXIT_SUCCESS;
}
