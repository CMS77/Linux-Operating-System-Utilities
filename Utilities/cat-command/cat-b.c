#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    FILE *file;
    char buffer[BUFFER_SIZE];
    size_t bytesRead;
    int lineNumber = 1;
    int nonEmptyLine = 0;
    int isFirstLine = 1; // Flag to track the first line

    if (argc != 2) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return 1;
    }

    file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        // Iterate through the buffer and print each character
        for (size_t i = 0; i < bytesRead; i++) {
            // If it's the start of a new line, print the line number
            if (isFirstLine || buffer[i - 1] == '\n') {
                if (nonEmptyLine)
                    printf("%6d  ", lineNumber++);
                nonEmptyLine = 0;
                isFirstLine = 0; // No longer the first line
            }
            // Print the current character
            putchar(buffer[i]);
            if (buffer[i] != '\n' && buffer[i] != ' ' && buffer[i] != '\t')
                nonEmptyLine = 1;
        }
    }

    fclose(file);
    return 0;
}