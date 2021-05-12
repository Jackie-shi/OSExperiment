//
// Created by 张运宸 on 2021/5/10.
// cp -r achieved by C
//

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

int file2file(char *source_file, char *destination_file, struct stat *pointer_stat);
int dir2dir(char *source_path, char *destination_path);
int cp2where(char *source_path, char *destination_path, struct stat source_stat, struct stat destination_stat);

int main(int argc, char *argv[]){
    // 使用lstat获取文件状态，判断为文件还是文件夹
    struct stat source_stat, destination_stat;
    int ret = 0;
    if (argc != 3){
        printf("error!");
        exit(EXIT_FAILURE);
    }
    else{
        // 获取源文件状态
        if (-1 == lstat(argv[1], &source_stat)){
            printf("error!");
            exit(EXIT_FAILURE);
        }
        ret = cp2where(argv[1], argv[2], source_stat, destination_stat);
        return ret;
    }
}



int file2file(char *source_file, char *destination_file, struct stat *pointer_stat){
    int source_file_description = open(source_file, O_RDONLY);
    if (-1 == source_file_description){
        printf("error!");
        exit(EXIT_FAILURE);
    }
    int destination_file_description = open(
            destination_file,
            O_WRONLY | O_CREAT | O_TRUNC,
            pointer_stat->st_mode
    );
    if (-1 == destination_file_description){
        printf("error!");
        exit(EXIT_FAILURE);
    }
    int source_file_size = lseek(source_file_description, 0, SEEK_END); // 获取源文件大小
    lseek(source_file_description, 0, SEEK_SET); // 调整文件指针
    char *read_buffer = (char *)malloc(sizeof(char)*source_file_size+1);
    int read_size = read(source_file_description, read_buffer, source_file_size);
    if (read_size == source_file_size){

    }
    int write_size = write(destination_file_description, read_buffer, read_size);
    if (read_size == write_size){

    }
    if (source_file_description > 0){
        close(source_file_description);
    }
    if (destination_file_description > 0){
        close(destination_file_description);
    }
    return 0;
}



int dir2dir(char *source_path, char *destination_path){
    DIR *source_dir = NULL;
    struct dirent *source_dirent_pointer = NULL;
    struct stat source_stat;
    source_dir = opendir(source_path);
    memset(&source_stat, 0, sizeof(source_dir));
    char source_buffer[4096], destination_buffer[4096]; // 文件路径最大长度
    while ((source_dirent_pointer = readdir(source_dir)) != NULL) {
        // 竟然写了一堆if,[裂开]
        if (source_dirent_pointer->d_name[0] == '.' && source_dirent_pointer->d_name[1] == 0){
            continue;
        }
        if (source_dirent_pointer->d_name[0] == '.' && source_dirent_pointer->d_name[1] == '.'){
            continue;
        }
        if (source_dirent_pointer->d_name[0] != '.' || (source_dirent_pointer->d_name[0] == '.' && source_dirent_pointer->d_name[1] != 0)) {
            printf("%s\n", source_dirent_pointer->d_name);
            sprintf(source_buffer, "%s/%s", source_path, source_dirent_pointer->d_name);
            // 获取文件属性
            if (-1 == lstat(source_buffer, &source_stat)) {
                printf("error111!\n");
                exit(EXIT_FAILURE);
            }
            sprintf(destination_buffer, "%s/%s", destination_path, source_dirent_pointer->d_name);
            if (S_ISDIR(source_stat.st_mode)) {
                if (-1 == access(destination_buffer, F_OK)) {
                    mkdir(destination_buffer, source_stat.st_mode);
                }
                dir2dir(source_buffer, destination_buffer); // 递归调用，处理文件树
            } else {
                file2file(source_buffer, destination_buffer, &source_stat);
            }
        }
    }
    closedir(source_dir);
    return 0;
}



int cp2where(char *source_path, char *destination_path, struct stat source_stat, struct stat destination_stat){
    if (S_ISDIR(source_stat.st_mode)){
        if (0 != access(destination_path, F_OK)){
            mkdir(destination_path, source_stat.st_mode);
        }
        else{
            if (-1 == lstat(destination_path, &destination_stat)){
                printf("error!");
                exit(EXIT_FAILURE);
            }
            if (!S_ISDIR((destination_stat.st_mode))){
                printf("destination is existed file");
                exit(EXIT_FAILURE);
            }
            else{
                int len;
                char *src_pointer;
                char buffer[4096];
                len = strlen(source_path);
                src_pointer = source_path + (len - 1);
                // 通过数组地址前移
                while (src_pointer >= source_path && *src_pointer != '/'){
                    src_pointer--;
                }
                src_pointer++;
                memset(buffer, 0, 4096);
                sprintf(buffer, "%s/%s", destination_path, src_pointer);
                if (access(buffer, F_OK) != 0){
                    mkdir(buffer, 0755);
                }
                dir2dir(source_path, buffer);
                return 0;
            }
        }
        dir2dir(source_path, destination_path);
        return 0;
    }
    else{
        if (0 == access(destination_path, F_OK)){
            if (-1 == lstat(destination_path, &destination_stat)){
                printf("error!");
                exit(EXIT_FAILURE);
            }
            if (S_ISDIR(destination_stat.st_mode)){
                int len;
                char *src_pointer;
                char buffer[4096];
                len = strlen(source_path);
                src_pointer = source_path + (len - 1);
                while (src_pointer >= source_path && *src_pointer != '/'){
                    src_pointer--;
                }
                src_pointer++;
                memset(buffer, 0, 4096);
                sprintf(buffer, "%s/%s", destination_path, src_pointer);
                file2file(source_path, buffer, &source_stat);
                return 0;
            }
            else{
                file2file(source_path, destination_path, &source_stat);
                return 0;
            }
        }
        else{
            int len;
            char *src_pointer;
            char buffer[4096];
            mkdir(destination_path, 0755);
            len = strlen(source_path);
            src_pointer = source_path + (len - 1);
            while (src_pointer >= source_path && *src_pointer != '/'){
                src_pointer--;
            }
            src_pointer++;
            memset(buffer, 0, 4096);
            sprintf(buffer, "%s/%s", destination_path, src_pointer);
            file2file(source_path, buffer, &destination_stat);
            return 0;
        }
    }
}
