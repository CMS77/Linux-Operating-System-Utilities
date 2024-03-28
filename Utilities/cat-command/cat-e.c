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

    int current_char;
    int previous_char = ' '; // Initialize to a non-newline character

    // Read characters until end of file
    while ((current_char = fgetc(file)) != EOF) {
        // Print character
        putchar(current_char);
        
        // Add $ character at the end of the line
        if (current_char == '\n') {
            putchar('$');
        }

        previous_char = current_char;
    }

    // Close the file
    fclose(file);
    return 0;
}
