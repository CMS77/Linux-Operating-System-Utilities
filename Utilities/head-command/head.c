#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void print_options() {
    printf("\nOptions:\n\n");
    printf("  <file>...            : Output the first 5 lines\n");
    printf("  -n <num> <file>...   : Output the first <num> lines\n");
    printf("  -q <file>...         : Never output headers giving file names\n");
    printf("  -c <num> <file>...   : Output the first <num> bytes\n");
    printf("  -v <file>...         : Always output headers giving file names\n");
    printf("  --help               : Display this help message\n\n");
}

void print_head_lines(FILE *file, int n) {
    char buffer[1024];
    int lines_printed = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL && lines_printed < n) {
        printf("%s", buffer);
        lines_printed++;
    }
}

void print_head_bytes(FILE *file, int n) {
    char buffer[1024];
    int bytes_read = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL && bytes_read < n) {
        int len = strlen(buffer);
        if (bytes_read + len > n) {
            int remaining_bytes = n - bytes_read;
            fwrite(buffer, 1, remaining_bytes, stdout);
            break;
        } else {
            printf("%s", buffer);
            bytes_read += len;
        }
    }
}

int main(int argc, char *argv[]) {
    int lines = 5; // -n
    int bytes = -1; // -c
    int no_header = 0; // -q
    int header = 0; // -v

    // Iterate over command-line arguments
    for (int i = 1; i < argc; i++) {
        // Check if the argument is an option
        if ( argv[i][0] == '-') {
            if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
                lines = atoi(argv[i + 1]);
                i++;
            } else if (strcmp(argv[i], "-q") == 0) {
                no_header = 1;
            } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
                bytes = atoi(argv[i + 1]);
                i++;
            } else if (strcmp(argv[i], "-v") == 0) {
                header = 1;
            } else if (strcmp(argv[i], "--help") == 0) {
                print_options();
                return EXIT_SUCCESS;
            } else {
                printf("Invalid option: %s. Check --help.\n", argv[i]);
                return EXIT_FAILURE;
            }
        } else {
            
            FILE *file = fopen(argv[i], "r");
            if (file == NULL) {
                perror("Error opening file");
                return EXIT_FAILURE;
            }
                
            if (header) {
                printf("\n==> %s <==\n\n", argv[i]);
            }

            if (bytes != -1) {
                print_head_bytes(file, bytes);
            } else {
                print_head_lines(file, lines);
            }

            fclose(file);
        
        } 
    }
    return EXIT_SUCCESS;
}