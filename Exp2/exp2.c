//
// Created by 张运宸 on 2021/5/10.
// The Producer-consumer Problem
//
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N 5
#define PRODUCER_NUM 3
#define CONSUMER_NUM 4

typedef char item;
item BUFFER[N];
char production, consumption;
int in = 0, out = 0;
sem_t empty, full, mutex;

void* product(int *arg){
    int producer_id = *(int *)arg;
    while (1){
        char production = rand() % 26 + 'A';
        sem_wait(&empty);
        sem_wait(&mutex);
        BUFFER[in] = production;
        in = (in + 1) % N;
        usleep(5);
        printf("Producer %d has produced %c in position %d\n", producer_id, production, in);
        sem_post(&mutex);
        sem_post(&full);
    }
}

void* consume(int *arg){
    int consumer_id = *(int *)arg;
    while (1){
        sem_wait(&full);
        sem_wait(&mutex);
        consumption = BUFFER[out];
        out = (out + 1) % N;
        usleep(5);
        printf("Consumer %d has consumed %c in position %d\n", consumer_id, consumption, out);
        sem_post(&mutex);
        sem_post(&empty);
    }
}

int main(){
    srand(0);
    // pshared 控制信号量类型;
    // value 信号量初始值
    // init
    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, N);
    // 创建生产者和消费者线程
    pthread_t consumer_threads[CONSUMER_NUM], producer_threads[PRODUCER_NUM];
    for (int j = 0; j < PRODUCER_NUM; ++j) {
        pthread_create(&producer_threads[j], NULL, product, &j);
    }
    for (int i = 0; i < CONSUMER_NUM; ++i) {
        pthread_create(&consumer_threads[i], NULL, consume, &i);
    }
    // wait
    for (int l = 0; l < PRODUCER_NUM; ++l) {
        pthread_join(producer_threads[l], NULL);
    }
    for (int k = 0; k < CONSUMER_NUM; ++k) {
        pthread_join(consumer_threads[k], NULL);
    }
    return 0;
}