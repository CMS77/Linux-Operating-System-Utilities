#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>  // Include for strcmp
#include <limits.h>

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
    struct passwd *pwd = getpwuid(statbuf->st_uid);
    printf(" %s", (pwd != NULL) ? pwd->pw_name : "unknown");

    // Print group name
    struct group *grp = getgrgid(statbuf->st_gid);
    printf(" %s", (grp != NULL) ? grp->gr_name : "unknown");

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

    // Check if the '-l' argument is provided
    int long_format = 0;
    if (argc >= 2 && strcmp(argv[1], "-l") == 0) {
        long_format = 1;
        if (argc < 3) {
            directory = opendir(".");
        } else {
            directory = opendir(argv[2]);
        }
    } else {
        if (argc < 2) {
            directory = opendir(".");
        } else {
            directory = opendir(argv[1]);
        }
    }

    // Check if directory opened successfully
    if (directory == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    // Read directory entries
    while ((entry = readdir(directory)) != NULL) {
        struct stat statbuf;
        char filename[PATH_MAX];
        snprintf(filename, sizeof(filename), "%s/%s", (argc < 2 || (long_format && argc < 3)) ? "." : argv[(long_format) ? 2 : 1], entry->d_name);

        if (stat(filename, &statbuf) == -1) {
            perror("Error getting file status");
            continue;
        }

        if (long_format) {
            print_file_info(entry->d_name, &statbuf);
        } else {
            printf("%s\n", entry->d_name);
        }
    }

    // Close directory
    closedir(directory);
    
    return 0;
}
