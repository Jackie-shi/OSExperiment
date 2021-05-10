//
// Created by 张运宸 on 2021/5/10.
// pipe IPC
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(){
    // fildes[0] 读管道 fildes[1] 写管道
    int fildes[2];
    if (-1 == pipe(fildes)){
        return -1;
    }

    pid_t process_1, process_2;

    process_1 = fork();
    if (-1 == process_1){
        // error
        return -1;
    }
    else if (0 == process_1){
        // child process
        char* massage_1 = "Child process 1 is sending a message!\n";
        close(fildes[0]);
        write(fildes[1], massage_1, strlen(massage_1));
        exit(0);
    }
    else {
        // father process
        // pid_t waitpid(pid_t pid, int *status, int options);
        // status==NULL, 父进程不关心子进程的终止状态, status!=NULL, 子进程终止后，存放在此单元
        // options 提供选项控制waitpid
        waitpid(process_1, NULL, 0);
    }

    process_2 = fork();
    if (-1 == process_2){
        return -1;
    }
    else if (0 == process_2){
        char* massage_2 = "Child process 2 is sending a message!\n";
        close(fildes[0]);
        write(fildes[1], massage_2, strlen(massage_2));
        exit(0);
    }
    else{
        waitpid(process_2, NULL, 0);
    }

    char massage_pipe[100];
    close(fildes[1]);
    read(fildes[0], massage_pipe, 100);
    printf("%s", massage_pipe);
    return 0;
}