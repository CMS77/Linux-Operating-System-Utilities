#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void print_options() {
    printf("\nOptions:\n\n");
    printf("  <file>               : Output the last 5 lines.\n");
    printf("  -n <num> <file>      : Output the last <num> lines.\n");
    printf("  -f <file>            : Output file while modified.\n");
    printf("  -q <file>            : Never output headers file.\n");
    printf("  -r <file>            : Output file in reverse.\n");
    printf("  --help               : Display this help message.\n\n");
}

void print_lines(FILE *file, int n, int reverse, int reverse_alone) {
    char buffer[1024];
    char **lines = NULL;
    int line_count = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        lines = realloc(lines, sizeof(char *) * (line_count + 1));
        lines[line_count] = strdup(buffer);
        line_count++;
    }

    if (reverse_alone) {
        for (int i = line_count - 1; i >= 0; i--) {
            printf("%s\n", lines[i]);
            free(lines[i]);
        }
    } else if (reverse) {
        int start = (line_count > n) ? line_count - n : 0;
        for (int i = line_count - 1; i >= start; i--) {
            printf("%s\n", lines[i]);
            free(lines[i]);
        }
    } else {
        int start = (line_count > n) ? line_count - n : 0;
        for (int i = start; i < line_count; i++) {
            printf("%s\n", lines[i]);
            free(lines[i]);
        }
    }

    free(lines);
}

int main(int argc, char *argv[]) {
    int lines = 5;
    int file_mod = 0; // -f
    int no_header = 0; // -q
    int reverse = 0; // -r & -n
    int reverse_alone = 0; // -r
    int file_count = 0; 

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
                lines = atoi(argv[i + 1]);
                i++;
            } else if (strcmp(argv[i], "-f") == 0) {
                file_mod = 1;
            } else if (strcmp(argv[i], "-q") == 0) {
                no_header = 1;
            } else if (strcmp(argv[i], "-r") == 0) {
                reverse = 1;
            } else if (strcmp(argv[i], "--help") == 0) {
                print_options();
                return EXIT_SUCCESS;
            } else {
                printf("Invalid option: %s. Check --help.\n", argv[i]);
                return EXIT_FAILURE;
            }
        } else {
            file_count++;
            FILE *file = fopen(argv[i], "r");
            if (file == NULL) {
                perror("Error opening file");
                return EXIT_FAILURE;
            }

            struct stat st;
            if (stat(argv[i], &st) != 0) {
                perror("Error getting file status");
                return EXIT_FAILURE;
            }
            time_t last_mtime = st.st_mtime;

            if (file_mod) {
                while (1) {
                    if (stat(argv[i], &st) != 0) {
                        perror("Error getting file status");
                        fclose(file);
                        return EXIT_FAILURE;
                    }
                    if (st.st_mtime != last_mtime) {
                        fseek(file, 0, SEEK_SET);
                        printf("----------\n");
                        print_lines(file, lines, reverse, reverse && lines == 5);
                        fflush(stdout);
                        last_mtime = st.st_mtime;
                    }
                    usleep(100000);
                }
            
            } 
            
            if (!no_header && (file_count > 1)) {
                printf("\n==> %s <==\n\n", argv[i]);
                print_lines(file, lines, reverse, reverse_alone);
            
            }else {
                reverse_alone = reverse && (lines == 5);
                print_lines(file, lines, reverse, reverse_alone);
            }

            fclose(file);
        }
    }
    return EXIT_SUCCESS;
}
