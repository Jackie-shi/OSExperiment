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

int main(int argc, char *argv[]){
    struct stat source_stat, destination_stat;
    char buffer[4096];
    int ret = 0;
    if (argc != 3){
        printf("error!");
        exit(EXIT_FAILURE);
    }
    else{
        if (-1 == lstat(argv[1], &source_stat)){
            printf("error!");
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(source_stat.st_mode)){
            if (access(argv[2], F_OK) != 0){
                // 没有目标文件夹的情况
                ret = mkdir(argv[2], source_stat.st_mode);
            }
            else{
                if (-1 == lstat(argv[2], &destination_stat)){
                    printf("error!");
                    exit(EXIT_FAILURE);
                }
                if (!S_ISDIR(destination_stat.st_mode)){
                    printf("destination is exist file");
                    exit(EXIT_FAILURE);
                }
                else{
                    // 获取路径最后一个字，倒序找出文件名
                    int len;
                    char *scr_pointer;
                    len = strlen(argv[1]);
                    scr_pointer = argv[1] + (len - 1);
                    while (scr_pointer >= argv[1] && *scr_pointer != '/'){
                        scr_pointer--;
                    }
                    scr_pointer ++;
                    memset(buffer, 0, 4096);
                    sprintf(buffer, "%s/%s", argv[2], scr_pointer);
                    if (access(buffer, F_OK) != 0){
                        ret = mkdir(buffer, 0755); //mkdir 默认755
                    }
                    ret = dir2dir(argv[1], buffer);
                    return ret;
                }
            }
            ret = dir2dir(argv[1], argv[2]);
            return ret;
        }
        else {
            if (0 == access(argv[2], F_OK)){
                if (-1 == lstat(argv[2], &destination_stat)){
                    printf("error!");
                    exit(EXIT_FAILURE);
                }
                if (S_ISDIR(destination_stat.st_mode)){
                    int len;
                    char *scr_pointer;
                    len = strlen(argv[1]);
                    scr_pointer = argv[1] + (len - 1);
                    while (scr_pointer >= argv[1] && *scr_pointer != '/'){
                        scr_pointer--;
                    }
                    scr_pointer++;
                    memset(buffer, 0, 4096);
                    sprintf(buffer, "%s/%s", argv[2], scr_pointer);
                    ret = file2file(argv[1], buffer, &source_stat);
                    return ret;
                }
                else{
                    ret = file2file(argv[1], argv[2], &source_stat);
                    return ret;
                }
            }
            else{
                int len;
                char *src_pointer;
                ret = mkdir(argv[2], 0755);
                len = strlen(argv[1]);
                src_pointer = argv[1] + (len - 1);
                while (src_pointer >= argv[1] && *src_pointer != '/'){
                    src_pointer--;
                }
                src_pointer++;
                memset(buffer, 0, 4096);
                sprintf(buffer, "%s/%s", argv[2], src_pointer);
                ret = file2file(argv[1], buffer, &destination_stat);
                return ret;
            }
        }
    }
}


