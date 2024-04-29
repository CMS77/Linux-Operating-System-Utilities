#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void print_options() {
    printf("\nOptions:\n");
    printf("  -n <num>    : Output the first <num> lines\n");
    printf("  -q          : Never output headers giving file names\n");
    printf("  -c <num>    : Output the first <num> bytes\n");
    printf("  -v          : Always output headers giving file names\n");
    printf("  --help      : Display this help message\n");
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
            // Print only required part of the line
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
    FILE *file = NULL;
    int lines = 10; // -n
    int bytes = -1; // -c
    int no_header = 0; // -q
    int header = 0; // -v
    int file_arg_encountered = 0;

    for (int i = 1; i < argc; i++) {
        if (!file_arg_encountered && argv[i][0] == '-') {
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
                printf("Invalid option: %s\n", argv[i]);
                return EXIT_FAILURE;
            }
        } else {
            if (!file_arg_encountered) {
                file = fopen(argv[i], "r");
                if (file == NULL) {
                    perror("Error opening file");
                    return EXIT_FAILURE;
                }
                file_arg_encountered = 1;
            } else {
                printf("Invalid Command\n");
                return EXIT_FAILURE;
            }
        }
    }

    if (file == NULL) {
        printf("No file provided\n");
        return EXIT_FAILURE;
    }

    if (header) {
        printf("\n|| %s ||\n\n", argv[argc - 1]);
    }

    if (bytes != -1) {
        print_head_bytes(file, bytes);
    } else {
        print_head_lines(file, lines);
    }

    fclose(file);
    return EXIT_SUCCESS;
}
