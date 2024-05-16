#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void move_file(const char *source, const char *destination, int interactive, int verbose, int no_clobber, int backup) {
    if (interactive) {
        printf("Do you want to move '%s' to '%s'? (y/n): ", source, destination);
        char response;
        scanf(" %c", &response);
        if (response != 'y') {
            printf("Move operation cancelled.\n");
            return;
        }
    }

    if (no_clobber && access(destination, F_OK) == 0) {
        printf("Destination file '%s' already exists. Not overwritten.\n", destination);
        return;
    }

    if (backup) {
        char backup_name[256];
        snprintf(backup_name, sizeof(backup_name), "%s~", destination);
        if (rename(destination, backup_name) != 0) {
            perror("Backup failed");
            return;
        }
    }

    if (rename(source, destination) != 0) {
        perror("Error moving file");
        return;
    }

    if (verbose) {
        printf("'%s' -> '%s'\n", source, destination);
    }
}

int main(int argc, char *argv[]) {
    int interactive = 0;
    int verbose = 0;
    int no_clobber = 0;
    int backup = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            interactive = 1;
        } else if (strcmp(argv[i], "-n") == 0) {
            no_clobber = 1;
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "--backup") == 0) {
            backup = 1;
        } else if (strcmp(argv[i], "--help") == 0) {
            printf("Usage: mv [OPTION]... SOURCE DEST\n");
            printf("Move SOURCE to DEST.\n\n");
            printf("Options:\n");
            printf("  -i              prompt before overwriting\n");
            printf("  -n              do not overwrite an existing file\n");
            printf("  -v              explain what is being done\n");
            printf("  --backup        make a backup of each existing destination file\n");
            printf("  --help          display this help and exit\n");
            return 0;
        }
    }

    if (argc < 3) {
        printf("mv: missing file operand\n");
        printf("Try 'mv --help' for more information.\n");
        return 1;
    }

    const char *source = argv[argc - 2];
    const char *destination = argv[argc - 1];

    move_file(source, destination, interactive, verbose, no_clobber, backup);

    return 0;
}
