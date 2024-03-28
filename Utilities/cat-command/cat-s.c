#include <stdio.h>

int main(int argc, char *argv[]) {
    // Check if file argument is provided
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // Open the file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error");
        return 1;
    }

    int prev_char = '\n'; // Initialize with newline to handle leading blank lines
    int current_char;

    // Read characters until end of file
    while ((current_char = fgetc(file)) != EOF) {
        // Print only if not consecutive blank lines
        if (!(prev_char == '\n' && current_char == '\n')) {
            putchar(current_char);
        }
        prev_char = current_char;
    }

    // Close the file
    fclose(file);
    return 0;
}
