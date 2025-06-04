#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BASE_FILENAME "a"
#define LINK_PREFIX "link_"
#define TEMP_DIR_TEMPLATE "symlink_test_XXXXXX"

int main() {
    char temp_dir[20];
    char base_file_path[256];
    char link_path_current[256];
    char link_path_next[256];
    int depth = 0;

    strcpy(temp_dir, TEMP_DIR_TEMPLATE);
    if (mkdtemp(temp_dir) == NULL) {
        perror("Error creating temporary directory");
        return EXIT_FAILURE;
    }
    printf("Created temporary directory: %s\n", temp_dir);

    snprintf(base_file_path, sizeof(base_file_path), "%s/%s", temp_dir, BASE_FILENAME);
    FILE *base_file = fopen(base_file_path, "w");
    if (base_file == NULL) {
        perror("Error creating base file");
        rmdir(temp_dir);
        return EXIT_FAILURE;
    }
    fclose(base_file);

    snprintf(link_path_current, sizeof(link_path_current), "%s/%s%d", temp_dir, LINK_PREFIX, depth);
    if (symlink(BASE_FILENAME, link_path_current) == -1) {
         perror("Error creating initial symlink");
         unlink(base_file_path);
         rmdir(temp_dir);
         return EXIT_FAILURE;
    }
    
    depth = 1;

    while (1) {
        snprintf(link_path_next, sizeof(link_path_next), "%s/%s%d", temp_dir, LINK_PREFIX, depth);
        if (symlink(link_path_current + strlen(temp_dir) + 1, link_path_next) == -1) {
            perror("Error creating symlink");
            break;
        }
        
        int fd = open(link_path_next, O_RDONLY);
        if (fd == -1) {
            if (errno == ELOOP) {
                printf("Reached max symlink recursion depth.\n");
            } else {
                 perror("Error opening symlink");
            }
            unlink(link_path_next);
            break;
        }
        close(fd);

        strcpy(link_path_current, link_path_next);
        depth++;
    }

    printf("Cleaning up created files...\n");
    char cleanup_path[256];
    for (int i = 0; i < depth; ++i) {
        snprintf(cleanup_path, sizeof(cleanup_path), "%s/%s%d", temp_dir, LINK_PREFIX, i);
        if (unlink(cleanup_path) == -1 && errno != ENOENT) {
             perror("Error cleaning up symlink");
        }
    }
    snprintf(cleanup_path, sizeof(cleanup_path), "%s/%s", temp_dir, BASE_FILENAME);
     if (unlink(cleanup_path) == -1 && errno != ENOENT) {
        perror("Error cleaning up base file");
    }

    if (rmdir(temp_dir) == -1) {
        perror("Error removing temporary directory");
    }

    printf("Symlink recursion depth: %d\n", depth -1);

    return 0;
} 