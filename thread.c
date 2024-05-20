#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>
#include <unistd.h>  

#define NUM_THREADS 4 // 固定线程数量
#define QUEUE_SIZE 10 // 队列大小

typedef struct {
    char url[256];
    char path[256];
} DownloadTask;

typedef struct {
    DownloadTask tasks[QUEUE_SIZE];
    int front;
    int rear;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} TaskQueue;

typedef struct {
    TaskQueue *queue;
    pthread_t threads[NUM_THREADS];
} ThreadPool;

void task_queue_init(TaskQueue *queue) {
    queue->front = queue->rear = queue->count = 0;
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);
}

void task_queue_push(TaskQueue *queue, DownloadTask task) {
    pthread_mutex_lock(&queue->lock);
    while (queue->count == QUEUE_SIZE) {
        pthread_cond_wait(&queue->not_full, &queue->lock);
    }
    queue->tasks[queue->rear] = task;
    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
    queue->count++;
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->lock);
}

DownloadTask task_queue_pop(TaskQueue *queue) {
    pthread_mutex_lock(&queue->lock);
    while (queue->count == 0) {
        pthread_cond_wait(&queue->not_empty, &queue->lock);
    }
    DownloadTask task = queue->tasks[queue->front];
    queue->front = (queue->front + 1) % QUEUE_SIZE;
    queue->count--;
    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->lock);
    return task;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

void* download_worker(void *arg) {
    TaskQueue *queue = (TaskQueue *)arg;
    while (1) {
        DownloadTask task = task_queue_pop(queue);

        CURL *curl;
        FILE *fp;
        CURLcode res;
        curl = curl_easy_init();
        if (curl) {
            fp = fopen(task.path, "wb");
            curl_easy_setopt(curl, CURLOPT_URL, task.url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            fclose(fp);
        }
        // 通知主线程或其他组件处理下载完成后的逻辑
        printf("Downloaded: %s to %s\n", task.url, task.path);
    }
    return NULL;
}

void thread_pool_init(ThreadPool *pool, TaskQueue *queue) {
    pool->queue = queue;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&pool->threads[i], NULL, download_worker, (void *)queue);
    }
}

void thread_pool_add_task(ThreadPool *pool, DownloadTask task) {
    task_queue_push(pool->queue, task);
}

int main() {
    TaskQueue queue;
    ThreadPool pool;

    task_queue_init(&queue);
    thread_pool_init(&pool, &queue);

    // 添加下载任务
    DownloadTask task1 = {"http://example.com/file1.txt", "file1.txt"};
    DownloadTask task2 = {"http://example.com/file2.txt", "file2.txt"};

    thread_pool_add_task(&pool, task1);
    thread_pool_add_task(&pool, task2);

    // 主线程等待（根据需要调整等待逻辑）
    for (int i = 0; i < 10; i++) {
        sleep(1);
    }

    return 0;
}

// TaskQueue: 用于存储下载任务的队列，包含任务数组、队列的前后指针和任务计数器，以及相应的互斥锁和条件变量。
// ThreadPool: 包含线程池的队列指针和线程数组。
// task_queue_init: 初始化任务队列。
// task_queue_push: 向任务队列添加任务。
// task_queue_pop: 从任务队列取出任务。
// write_data: curl写数据回调函数，将数据写入文件。
// download_worker: 工作线程函数，从队列中取出任务并执行下载操作。
// thread_pool_init: 初始化线程池并创建工作线程。
// thread_pool_add_task: 向线程池添加下载任务。
// main: 主函数，初始化任务队列和线程池，添加下载任务并等待任务完成。