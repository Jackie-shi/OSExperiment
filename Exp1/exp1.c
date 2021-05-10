//
// Created by 张运宸 on 2021/5/10.
//
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t _5_chopsticks[5];

_Noreturn void* eat_and_think(const int *arg){
    int left, rigth;
    int i;
    int philosopher = *(int *)arg;
    // 获取左右筷子编号
    i = philosopher + 1;
    left = i - 1;
    rigth = (i + 1) % 5 - 1;
    while (1){
        printf("Philosopher %d is thinking.\n", philosopher);
        // Thinking
        usleep(50);
        // Try eating
        pthread_mutex_lock(&_5_chopsticks[left]);
        int is_eating = pthread_mutex_trylock(&_5_chopsticks[rigth]); // 未加锁，返回0
        if (is_eating != 0){
            pthread_mutex_unlock(&_5_chopsticks[left]);
            continue;
        }
        printf("Philosopher %d is eating.\n", philosopher);
        pthread_mutex_unlock(&_5_chopsticks[left]);
        pthread_mutex_unlock(&_5_chopsticks[rigth]);
    }
}

int main(){
    pthread_t philosopher_t[5];
    // init mutex
    for (int i = 0; i < 5; i++) {
        pthread_mutex_init(&_5_chopsticks[i], NULL);
    }
    // init thread
    for (int j = 0; j < 5; j++) {
        pthread_create(&philosopher_t[j], NULL, (void *(*)(void *)) eat_and_think, &j);
    }
    // join
    for (int k = 0; k < 5; k++) {
        pthread_join(philosopher_t[k], NULL);
    }
    return 0;
}
