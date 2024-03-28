#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    FILE *source, *destination;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // Check if correct number of arguments provided
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open source file for reading
    source = fopen(argv[1], "rb");
    if (source == NULL) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    // Open destination file for writing
    destination = fopen(argv[2], "wb");
    if (destination == NULL) {
        perror("Error opening destination file");
        fclose(source);
        exit(EXIT_FAILURE);
    }

    // Copy data from source to destination
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, source)) > 0) {
        fwrite(buffer, 1, bytes_read, destination);
    }

    // Check for read error
    if (ferror(source) != 0) {
        perror("Error reading from source file");
    }

    // Check for write error
    if (ferror(destination) != 0) {
        perror("Error writing to destination file");
    }

    // Close files
    fclose(source);
    fclose(destination);

    return 0;
}
