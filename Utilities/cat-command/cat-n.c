#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    FILE *file;
    char buffer[BUFFER_SIZE];
    size_t bytesRead;
    int lineNumber = 1;

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
            if (lineNumber == 1 || buffer[i - 1] == '\n') {
                printf("%6d  ", lineNumber);
                lineNumber++;
            }
            // Print the current character
            putchar(buffer[i]);
        }
    }

    fclose(file);
    return 0;
}
