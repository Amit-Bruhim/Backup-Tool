#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

// Function declarations
void copy_directory(const char *src, const char *dst);
void create_hard_link(const char *src, const char *dst);
void copy_symlink(const char *src, const char *dst);

int main(int argc, char *argv[])
{
    // Check that there are exactly 2 arguments: source and destination directories
    if (argc != 3)
    {
        return 1;
    }

    // Try to open the source directory
    DIR *src = opendir(argv[1]);
    if (src == NULL)
    {
        perror("src dir");
        return 1;
    }

    // Check if destination directory already exists
    DIR *dst = opendir(argv[2]);
    if (dst != NULL)
    {
        perror("backup dir");
        closedir(src);
        return 1;
    }

    // Create destination directory
    mkdir(argv[2], 0777);

    // Start recursive directory copy
    copy_directory(argv[1], argv[2]);

    return 0;
}

// Create a hard link from src to dst
void create_hard_link(const char *src, const char *dst)
{
    link(src, dst);
}

// Create a symbolic link from src to dst
void copy_symlink(const char *src, const char *dst)
{
    symlink(src, dst);
}

// Recursively copy a directory, including files, subdirectories, and symbolic links
void copy_directory(const char *src, const char *dst)
{
    struct dirent *entry;
    struct stat statbuf;
    DIR *dir = opendir(src);
    char buffer[512];
    char dst_path[512];

    // Iterate through all entries in the source directory
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip hidden entries like "." and ".."
        if (entry->d_name[0] == '.')
        {
            continue;
        }

        // Build the full source path
        sprintf(buffer, "%s/%s", src, entry->d_name);

        // Get file/directory information
        lstat(buffer, &statbuf);

        // If it's a regular file, create a hard link in the destination
        if (S_ISREG(statbuf.st_mode))
        {
            sprintf(dst_path, "%s/%s", dst, entry->d_name);
            create_hard_link(buffer, dst_path);
        }
        // If it's a directory, create it and recursively copy its contents
        else if (S_ISDIR(statbuf.st_mode))
        {
            sprintf(dst_path, "%s/%s", dst, entry->d_name);
            mkdir(dst_path, 0777);
            copy_directory(buffer, dst_path);
        }
    }

    // Close and reopen the directory to handle symbolic links separately
    closedir(dir);
    dir = opendir(src);

    // Iterate again to copy symbolic links
    while ((entry = readdir(dir)) != NULL)
    {
        sprintf(dst_path, "%s/%s", dst, entry->d_name);
        sprintf(buffer, "%s/%s", src, entry->d_name);
        lstat(buffer, &statbuf);

        // If the entry is a symbolic link, copy it
        if (S_ISLNK(statbuf.st_mode))
        {
            char target[512];

            // Read the target path of the symlink
            ssize_t len = readlink(buffer, target, sizeof(target) - 1);
            target[len] = '\0';

            // Create the symlink in the destination directory
            sprintf(dst_path, "%s/%s", dst, entry->d_name);
            symlink(target, dst_path);
        }
    }
}
