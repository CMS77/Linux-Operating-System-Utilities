#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

void print_options() {
    printf("\nOptions:\n\n");
    printf("  <file>            : Display the content of file.\n");
    printf("  -n <file>         : Number all output lines.\n");
    printf("  -b <file>         : Number non-blank output lines.\n");
    printf("  -s <file>         : Suppress repeated empty output lines.\n");
    printf("  -E <file>         : Display $ at the end of each line.\n");
    printf("  --help            : Display this help message.\n\n");
}

void cat_file(FILE *file, int number_all, int number_non_blank, int suppress_empty, int display_end) {
    char line[MAX_LINE_LENGTH];
    int line_num = 1;
    int empty_line_seen = 0;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        
        if (suppress_empty && strcmp(line, "\n") == 0) {
            if (empty_line_seen == 1) continue;
            
            empty_line_seen = 1;
        } else {
            empty_line_seen = 0;
        }

        if (number_all || (number_non_blank && empty_line_seen == 0)) {
            printf("\n%6d ", line_num);
            line_num++;
        }

        if (display_end) {
            int eolFound = 0;
            for(int i = 0; i < MAX_LINE_LENGTH && eolFound == 0; i++){
                if(line[i] == '\n'){
                    line[i] = '$';
                    eolFound = 1;
                }
            }
            printf("%s\n", line);
        } else {
            printf("%s", line);
        }
    }
}

int main(int argc, char *argv[]) {
    int number_all = 0;
    int number_non_blank = 0;
    int suppress_empty = 0;
    int display_end = 0;

    if (argc == 1) {
        cat_file(stdin, number_all, number_non_blank, suppress_empty, display_end);
    } else {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--help") == 0) {
                print_options();
            } else if (strcmp(argv[i], "-n") == 0) {
                number_all = 1;
            } else if (strcmp(argv[i], "-b") == 0) {
                number_non_blank = 1, suppress_empty = 1;
            } else if (strcmp(argv[i], "-s") == 0) {
                suppress_empty = 1;
            } else if (strcmp(argv[i], "-E") == 0) {
                display_end = 1;
            } else {
                FILE *file = fopen(argv[i], "r");
                if (file == NULL) {
                    fprintf(stderr, "Cat: %s: No such file or directory\n", argv[i]);
                    continue;
                }
                cat_file(file, number_all, number_non_blank, suppress_empty, display_end);
                fclose(file);
            }
        }
    }
    return 0;
}