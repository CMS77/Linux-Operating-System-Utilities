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

void human_read_size(off_t size, char *buffer, size_t buffer_size) {
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;

    while (size > 1024 && unit_index < 4) {
        size /= 1024;
        unit_index++;
    }

    snprintf(buffer, buffer_size, "%ld %s", (long)size, units[unit_index]);
}


void print_file_info(const char *filename, const struct stat *statbuf) {
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

    struct passwd *pw = getpwuid(statbuf->st_uid);
    printf(" [%s]", (pw) ? pw->pw_name : "unknown");

    struct group *gr = getgrgid(statbuf->st_gid);
    printf(" [%s]", (gr) ? gr->gr_name : "unknown");

    printf(" [%lld bytes]", (long long)statbuf->st_size);

    char timebuf[80];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&statbuf->st_mtime));
    printf(" [%s]", timebuf);

    printf(" [%s]\n", filename);
}

void print_args() {
    printf("Arguments for ls command:\n");
    printf("  -a          : Show hidden files\n");
    printf("  -h          : Show file sizes in human-readable format\n");
    printf("  -l          : Show detailed file information(Type, Owner, Group, Size, Modification time, Name)\n");
    printf("  --s         : Sort files by size\n");
    printf("  --help      : Display this help message\n");
}

int main(int argc, char *argv[]) {
    DIR *directory;
    struct dirent *entry;
    struct stat statbuf;

    int hide = 0;     // -a 
    int human_size = 0;  // -h 
    int detail_file = 0;     // -l
    int sort = 0;    // --s 

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            hide = 1;
        } else if (strcmp(argv[i], "-h") == 0) {
            human_size = 1;
        } else if (strcmp(argv[i], "-l") == 0) {
            detail_file = 1;
        } else if (strcmp(argv[i], "--s") == 0) {
            sort = 1;
        } else if (strcmp(argv[i], "--help") == 0) {
            print_args();
             return EXIT_SUCCESS;
        }
    }

     for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            return EXIT_SUCCESS;
        }
    }

    if (argc < 2 || (argc == 2 && (hide || human_size || detail_file || sort ))) {
        directory = opendir(".");
    } else {
        directory = opendir(argv[1]);
    }


    if (directory == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    FileInfo *file_infos = malloc(sizeof(FileInfo) * 1000);
    if (file_infos == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    int num_files = 0;


    while ((entry = readdir(directory)) != NULL) {
        if (!hide && entry->d_name[0] == '.') {
            continue;
        }

        if (fstatat(dirfd(directory), entry->d_name, &statbuf, 0) == -1) {
            perror("Error getting file status");
            continue;
        }
        file_infos[num_files].name = strdup(entry->d_name);
        file_infos[num_files].size = statbuf.st_size;
        num_files++;
    }

    if (sort) {
        qsort(file_infos, num_files, sizeof(FileInfo), compare_file_sizes);
    }

    if (detail_file) {
        for (int i = 0; i < num_files; i++) {
            if (hide || file_infos[i].name[0] != '.') {
                if (lstat(file_infos[i].name, &statbuf) == -1) {
                    perror("Error getting file status");
                    continue;
                }
                print_file_info(file_infos[i].name, &statbuf);
            }
        }
    } else if (human_size){
        for (int i = 0; i < num_files; i++) {
            printf("%s", file_infos[i].name);
            if (human_size) {
                char size_buffer[20];
                human_read_size(file_infos[i].size, size_buffer, sizeof(size_buffer));
                printf(" (%s)", size_buffer);
            }
            printf("\n");
        }

    } else if (sort){
    
        for (int i = 0; i < num_files; i++) {
            printf("%-20s %lld bytes\n", file_infos[i].name, (long long)file_infos[i].size);
            free(file_infos[i].name);
        }
    

    } else {
        for (int i = 0; i < num_files; i++) {
            printf("%s\n", file_infos[i].name);
        }
    }

    free(file_infos);

    closedir(directory);

    return 0;
}
