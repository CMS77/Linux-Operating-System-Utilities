#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


void print_options() {
    printf("\nOptions:\n\n");
    printf("  <file>...            : Output the last 5 lines\n");
    printf("  -n <num> <file>...   : Output the last <num> lines\n");
    printf("  -f <file>...         : Output file while modified\n");
    printf("  -q <file>...         : Never output headers file\n");
    printf("  -r <file>...         : Output file in reverse\n");
    printf("  --help               : Display this help message\n\n");
}

void print_last_lines(FILE *file, int n) {
    char buffer[1024];
    char *lines[n];
    int line_count = 0;
    int index = 0;

    for (int i = 0; i < n; i++)
        lines[i] = NULL;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (lines[index] != NULL)
            free(lines[index]);

        lines[index] = strdup(buffer);
        index = (index + 1) % n;
        line_count++;
    }

    int start = (line_count >= n) ? index : 0;
    for (int i = 0; i < n && i < line_count; i++) {
        if (lines[(start + i) % n] != NULL)
            printf("%s", lines[(start + i) % n]);
    }

    for (int i = 0; i < n; i++)
        if (lines[i] != NULL)
            free(lines[i]);
}

void print_reverse(FILE *file) {
    char buffer[1024];
    char **lines = NULL;
    int line_count = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        lines = realloc(lines, sizeof(char *) * (line_count + 1));
        lines[line_count] = strdup(buffer);
        line_count++;
    }

    for (int i = line_count - 1; i >= 0; i--) {
        printf("%s", lines[i]);
        free(lines[i]);
    }

    free(lines);
}

int main(int argc, char *argv[]) {
    int lines = 5; // -n
    int file_mod = 0; // -f
    int no_header = 0; // -q
    int reverse = 0; // -r

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
                FILE *file = fopen(argv[i], "r");
                if (file == NULL) {
                    perror("Error opening file");
                    return EXIT_FAILURE;
                }

                struct stat st;
                if (stat(argv[argc - 1], &st) != 0) {
                    perror("Error getting file status");
                    return EXIT_FAILURE;
                }
                time_t last_mtime = st.st_mtime;

                if (reverse) {
                    print_reverse(file);
                } else if (file_mod) {
                    print_last_lines(file, lines);

                    while (1) {
                        usleep(100000); 
                        if (stat(argv[argc - 1], &st) != 0) {
                            perror("Error getting file status");
                            fclose(file);
                            return EXIT_FAILURE;
                        }
                        if (st.st_mtime != last_mtime) {
                            fseek(file, 0, SEEK_SET);
                            system("clear"); 
                            print_last_lines(file, lines);
                            last_mtime = st.st_mtime;
                        }
                    }
                } else {
                    print_last_lines(file, lines);
                }

                fclose(file);
        }
    } 
    return EXIT_SUCCESS;
}