#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_POOL_SIZE 10
#define TASK_QUEUE_SIZE 100

typedef struct {
    int num;
    void (*function)(int);
} Task;

typedef struct {
    Task tasks[TASK_QUEUE_SIZE];
    int front;
    int rear;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} TaskQueue;

typedef struct {
    pthread_t threads[THREAD_POOL_SIZE];
    TaskQueue *taskQueue;
    int isRunning;
} ThreadPool;

void enqueue(TaskQueue *queue, Task task) {
    pthread_mutex_lock(&(queue->mutex));
    queue->tasks[queue->rear] = task;
    queue->rear = (queue->rear + 1) % TASK_QUEUE_SIZE;
    queue->count++;
    pthread_cond_signal(&(queue->cond));
    pthread_mutex_unlock(&(queue->mutex));
}

Task dequeue(TaskQueue *queue) {
    pthread_mutex_lock(&(queue->mutex));
    while (queue->count == 0) {
        pthread_cond_wait(&(queue->cond), &(queue->mutex));
    }
    Task task = queue->tasks[queue->front];
    queue->front = (queue->front + 1) % TASK_QUEUE_SIZE;
    queue->count--;
    pthread_mutex_unlock(&(queue->mutex));
    return task;
}

void *worker(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;
    while (pool->isRunning || pool->taskQueue->count > 0) {
        Task task = dequeue(pool->taskQueue);
        task.function(task.num);
    }
    return NULL;
}

void calculate_factorial(int num) {
    unsigned long long result = 1;
    for (int i = 1; i <= num; ++i) {
        result *= i;
    }
    printf("Factorial of %d is %llu\n", num, result);
}

void initThreadPool(ThreadPool *pool, TaskQueue *queue) {
    pool->taskQueue = queue;
    pool->isRunning = 1;
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        pthread_create(&(pool->threads[i]), NULL, worker, pool);
    }
}

void shutdownThreadPool(ThreadPool *pool) {
    pool->isRunning = 0;
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        pthread_cond_broadcast(&(pool->taskQueue->cond));
    }
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        pthread_join(pool->threads[i], NULL);
    }
}

void initTaskQueue(TaskQueue *queue) {
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;
    pthread_mutex_init(&(queue->mutex), NULL);
    pthread_cond_init(&(queue->cond), NULL);
}

int main() {
    TaskQueue taskQueue;
    ThreadPool threadPool;

    initTaskQueue(&taskQueue);
    initThreadPool(&threadPool, &taskQueue);

    for (int i = 0; i < 20; ++i) {
        Task task;
        task.num = i;
        task.function = calculate_factorial;
        enqueue(&taskQueue, task);
    }

    sleep(5); // Simulate some delay to allow tasks to be processed

    shutdownThreadPool(&threadPool);

    pthread_mutex_destroy(&(taskQueue.mutex));
    pthread_cond_destroy(&(taskQueue.cond));

    return 0;
}