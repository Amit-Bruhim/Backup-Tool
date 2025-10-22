#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

void copy_directory(const char *src, const char *dst);
void create_hard_link(const char *src, const char *dst);
void copy_symlink(const char *src, const char *dst);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        return 1;
    }
    DIR *src = opendir(argv[1]);
    if (src == NULL)
    {
        perror("src dir");
        return 1;
    }
    DIR *dst = opendir(argv[2]);
    if (dst != NULL)
    {
        perror("backup dir");
        closedir(src);
        return 1;
    }
    mkdir(argv[2], 0777);
    copy_directory(argv[1], argv[2]);
    return 0;
}

void create_hard_link(const char *src, const char *dst)
{
    link(src, dst);
}

void copy_symlink(const char *src, const char *dst)
{
    symlink(src, dst);
}

void copy_directory(const char *src, const char *dst)
{
    struct dirent *entry;
    struct stat statbuf;
    DIR *dir = opendir(src);
    char buffer[512];

    char dst_path[512];
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
        {
            continue;
        }
        sprintf(buffer, "%s/%s", src, entry->d_name);
        lstat(buffer, &statbuf);

        

        if (S_ISREG(statbuf.st_mode))
        {
            sprintf(dst_path, "%s/%s", dst, entry->d_name);
            create_hard_link(buffer, dst_path);
        }
        else if (S_ISDIR(statbuf.st_mode))
        {
            sprintf(dst_path, "%s/%s", dst, entry->d_name);
            mkdir(dst_path, 0777);
            copy_directory(buffer, dst_path);
        }
    }
    closedir(dir);
    dir = opendir(src);

    while ((entry = readdir(dir)) != NULL)
    {
        sprintf(dst_path, "%s/%s", dst, entry->d_name);
        sprintf(buffer, "%s/%s", src, entry->d_name);
        lstat(buffer, &statbuf);
        if (S_ISLNK(statbuf.st_mode))
        {
            char target[512];
            ssize_t len = readlink(buffer, target, sizeof(target) - 1);
            target[len] = '\0';
            sprintf(dst_path, "%s/%s", dst, entry->d_name);
            symlink(target, dst_path);
        }
    }
}