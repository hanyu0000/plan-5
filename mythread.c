// 实现一个简单的并发下载器
// 假设你正在开发一个下载器程序，需要同时下载多个文件。为了提高下载效率，你决定使用线程池来管理并发下载任务。
// 具体需求如下：
//     创建一个线程池，用于执行下载任务。
//     线程池应该具有一定数量的工作线程，例如固定数量的线程或者根据系统资源动态调整的线程池。
//     下载任务应该包含文件的URL地址和本地保存路径。
//     线程池应该能够接受下载任务，并将任务分配给可用的工作线程进行执行。
//     每个工作线程负责下载一个文件，并将下载的文件保存到指定的本地路径。
//     下载任务可以是耗时的操作，线程池应该能够同时处理多个下载任务，提高下载效率。
//     下载完成后，线程池应该通知主线程或其他组件进行后续处理或等待新任务。
// 任务要求
//     线程池的初始化和管理：创建线程池并管理其中的工作线程。
//     任务队列：用于接收和存储待执行的下载任务，线程池从任务队列中获取任务进行处理。
//     线程池调度：线程池需要实现任务调度算法，将下载任务分配给可用的工作线程进行执行。
//     下载任务的执行：每个工作线程负责下载一个文件，并将下载的文件保存到指定的本地路径。
//     下载完成后的处理：线程池需要通知主线程或其他组件进行后续处理，例如文件合并或其他操作。

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>
#include <unistd.h>  // 包含 sleep 函数的头文件
