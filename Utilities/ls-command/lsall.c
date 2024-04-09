#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h>

typedef struct {
    char *name;
    off_t size;
} FileInfo;

// Function to compare file sizes for sorting
int compare_file_sizes(const void *a, const void *b) {
    const FileInfo *fileA = (const FileInfo *)a;
    const FileInfo *fileB = (const FileInfo *)b;

    if (fileA->size < fileB->size) {
        return -1;
    } else if (fileA->size > fileB->size) {
        return 1;
    } else {
        return 0;
    }
}

// Function to convert file size to a human-readable format
void human_readable_size(off_t size, char *buffer, size_t buffer_size) {
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;

    while (size > 1024 && unit_index < 4) {
        size /= 1024;
        unit_index++;
    }

    snprintf(buffer, buffer_size, "%ld %s", (long)size, units[unit_index]);
}

// Function to print file info in long format (-l)
void print_file_info(const char *filename, const struct stat *statbuf) {
    // Print file type
    printf((S_ISDIR(statbuf->st_mode)) ? "d" : "-");
    printf((statbuf->st_mode & S_IRUSR) ? "r" : "-");
    printf((statbuf->st_mode & S_IWUSR) ? "w" : "-");
    printf((statbuf->st_mode & S_IXUSR) ? "x" : "-");
    printf((statbuf->st_mode & S_IRGRP) ? "r" : "-");
    printf((statbuf->st_mode & S_IWGRP) ? "w" : "-");
    printf((statbuf->st_mode & S_IXGRP) ? "x" : "-");
    printf((statbuf->st_mode & S_IROTH) ? "r" : "-");
    printf((statbuf->st_mode & S_IWOTH) ? "w" : "-");
    printf((statbuf->st_mode & S_IXOTH) ? "x" : "-");

    // Print number of hard links
    printf(" %hu", (unsigned short)statbuf->st_nlink);

    // Print owner name
    struct passwd *pw = getpwuid(statbuf->st_uid);
    printf(" %s", (pw) ? pw->pw_name : "unknown");

    // Print group name
    struct group *gr = getgrgid(statbuf->st_gid);
    printf(" %s", (gr) ? gr->gr_name : "unknown");

    // Print file size
    printf(" %lld", (long long)statbuf->st_size);

    // Print last modification time
    char timebuf[80];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&statbuf->st_mtime));
    printf(" %s", timebuf);

    // Print filename
    printf(" %s\n", filename);
}

int main(int argc, char *argv[]) {
    DIR *directory;
    struct dirent *entry;
    struct stat statbuf;

    int show_hidden = 0;     // -a option
    int human_readable = 0;  // -h option
    int long_format = 0;     // -l option
    int sort_by_size = 0;    // --s option

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            show_hidden = 1;
        } else if (strcmp(argv[i], "-h") == 0) {
            human_readable = 1;
        } else if (strcmp(argv[i], "-l") == 0) {
            long_format = 1;
        } else if (strcmp(argv[i], "--s") == 0) {
            sort_by_size = 1;
        }
    }

    // Open current directory if no directory provided as argument
    if (argc < 2 || (argc == 2 && (show_hidden || human_readable || long_format || sort_by_size))) {
        directory = opendir(".");
    } else {
        directory = opendir(argv[1]);
    }

    // Check if directory opened successfully
    if (directory == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for an array of FileInfo structures
    FileInfo *file_infos = malloc(sizeof(FileInfo) * 1000); // Assuming maximum of 1000 files
    if (file_infos == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    int num_files = 0;

    // Read directory entries and collect file information
    while ((entry = readdir(directory)) != NULL) {
        // Skip hidden files if -a option is not provided
        if (!show_hidden && entry->d_name[0] == '.') {
            continue;
        }

        // Get file information
        if (fstatat(dirfd(directory), entry->d_name, &statbuf, 0) == -1) {
            perror("Error getting file status");
            continue;
        }

        // Store file name and size in the file_infos array
        file_infos[num_files].name = strdup(entry->d_name); // strdup() to duplicate the string
        file_infos[num_files].size = statbuf.st_size;
        num_files++;
    }

    // Sort files by size if sort_by_size flag is set
    if (sort_by_size) {
        qsort(file_infos, num_files, sizeof(FileInfo), compare_file_sizes);
    }

    // Print file info based on options
    if (long_format) {
        // Print file info in long format if -l option is provided
        for (int i = 0; i < num_files; i++) {
            if (show_hidden || file_infos[i].name[0] != '.') {
                if (lstat(file_infos[i].name, &statbuf) == -1) {
                    perror("Error getting file status");
                    continue;
                }
                print_file_info(file_infos[i].name, &statbuf);
            }
        }
    } else if (human_readable){
        // Print file names with optional size in human-readable format
        for (int i = 0; i < num_files; i++) {
            printf("%s", file_infos[i].name);
            if (human_readable) {
                char size_buffer[20];
                human_readable_size(file_infos[i].size, size_buffer, sizeof(size_buffer));
                printf(" (%s)", size_buffer);
            }
            printf("\n");
        }
    } else if (sort_by_size){
        // Print sorted files with sizes if --s option is provided
    
        for (int i = 0; i < num_files; i++) {
            printf("%-20s %lld bytes\n", file_infos[i].name, (long long)file_infos[i].size);
            free(file_infos[i].name); // Free each duplicated string
        }
    

    } else {
        // Print normal listing of files
        for (int i = 0; i < num_files; i++) {
            printf("%s\n", file_infos[i].name);
        }
    }

    

    // Free allocated memory
    free(file_infos);

    // Close directory
    closedir(directory);

    return 0;
}
