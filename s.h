#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>

#define MAX_THREADS 64
#define MAX_PATH_LEN 257
#define MAX_SKIP_PATHS 10

int totalDirs = 0;
int totalFiles = 0;
pthread_mutex_t mutex;

typedef struct SearchConfig
{
    char root_path[MAX_PATH_LEN];                  // 要搜索的根目录
    char file_type[MAX_PATH_LEN];                  // 要搜索的文件类型，如 ".txt"、".cpp" 等
    int max_concurrency;                           // 最大并发数
    int max_depth;                                 // 最大搜索深度
    bool skip_hidden;                              // 是否跳过隐藏文件或目录
    char skip_paths[MAX_SKIP_PATHS][MAX_PATH_LEN]; // 要跳过的目录或文件的路径
} Task;

typedef struct Node
{
    char path[MAX_PATH_LEN];
    struct Node *next;
} Node;
Node *head = NULL;

void count(const char *path, const Task *config, int depth);
void *thread(void *arg);
void addToList(const char *path);
bool exists(const char *path);
void freeList();