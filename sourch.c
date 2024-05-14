#include "s.h"
int totalDirs = 0;
int totalFiles = 0;
int main(int argc, char *argv[])
{
    pthread_t threads[MAX_THREADS];
    int numThreads;
    int i;

    Task config;
    strcpy(config.root_path, argv[1]); // 第一个命令行参数为根目录路径
    if (argv[2] == NULL)
        strcpy(config.file_type, ".txt"); // 默认".txt"
    else
        strcpy(config.file_type, argv[2]); // 第二个命令行参数为文件类型
    config.max_concurrency = 4;            // 默认最大并发数为 4
    if (argv[3] == NULL)
        config.max_depth = -1; // 默认最大搜索深度为 -1，表示无限制
    else
        config.max_depth = atoi(argv[3]);
    config.skip_hidden = true;      // 默认跳过隐藏文件或目录
    config.skip_paths[0][0] = '\0'; // 默认不跳过任何路径

    if (argc < 2)
    {
        printf("参数数量错误\n");
        exit(1);
    }

    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        printf("线程init错误\n");
        exit(1);
    }
    // 多个线程进行处理
    numThreads = config.max_concurrency > MAX_THREADS ? MAX_THREADS : config.max_concurrency;
    for (i = 0; i < numThreads; i++)
        pthread_create(&threads[i], NULL, thread, (void *)&config);
    // 等待线程结束
    for (i = 0; i < numThreads; i++)
        pthread_join(threads[i],NULL);
    pthread_mutex_destroy(&mutex);
    printf("最大搜索深度:%s \n", argv[3]);
    printf("当前目录下%s文件数量: %d\n", config.file_type, totalFiles);
    printf("当前目录下文件夹数量: %d\n", totalDirs);

    return 0;
}
void *thread(void *arg)
{
    Task *config = (Task *)arg;
    int local_totalFiles = 0;
    count(config->root_path, config, 0);
}
void count(const char *path, const Task *config, int depth)
{
    DIR *dir;
    struct dirent *entry;
    char fullPath[MAX_PATH_LEN];

    dir = opendir(path);
    if (dir == NULL)
        return;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        // 文件夹
        if (entry->d_type == DT_DIR)
        {
            if (config->max_depth == -1 || depth < config->max_depth)
            {
                count(fullPath, config, depth + 1);
                pthread_mutex_lock(&mutex);
                totalDirs++;
                pthread_mutex_unlock(&mutex);
            }
        }
        // 文件
        if (entry->d_type == DT_REG && strstr(entry->d_name, config->file_type) != NULL)
        {
            pthread_mutex_lock(&mutex);
            totalFiles++;
            printf("文件路径: %s\n", fullPath);
            pthread_mutex_unlock(&mutex);
            
        }
    }
    closedir(dir);
}