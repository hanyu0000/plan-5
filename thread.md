# 线程池
### 实现一个简单的并发下载器
假设你正在开发一个下载器程序，需要同时下载多个文件。为了提高下载效率，你决定使用线程池来管理并发下载任务。具体需求如下：

    1.创建一个线程池，用于执行下载任务。
    2.线程池应该具有一定数量的工作线程，例如固定数量的线程或者根据系统资源动态调整的线程池。
    3.下载任务应该包含文件的URL地址和本地保存路径。
    线程池应该能够接受下载任务，并将任务分配给可用的工作线程进行执行。
    4.每个工作线程负责下载一个文件，并将下载的文件保存到指定的本地路径。
    5.下载任务可以是耗时的操作，线程池应该能够同时处理多个下载任务，提高下载效率。
    6.下载完成后，线程池应该通知主线程或其他组件进行后续处理或等待新任务。

任务要求：

    1.线程池的初始化和管理：创建线程池并管理其中的工作线程。
    2.任务队列：用于接收和存储待执行的下载任务，线程池从任务队列中获取任务进行处理。
    3.线程池调度：线程池需要实现任务调度算法，将下载任务分配给可用的工作线程进行执行。
    4.下载任务的执行：每个工作线程负责下载一个文件，并将下载的文件保存到指定的本地路径。
    5.下载完成后的处理：线程池需要通知主线程或其他组件进行后续处理，例如文件合并或其他操作。

具体思路：创建线程池->添加下载任务->添加任务到队列->实现->停止线程池

##### 创建线程池
```cpp
    Threadpool pool();
```

##### 添加下载任务

**创建下载任务结构体**
```cpp
    struct Task{
    string url;
    string localPath;
};
```

1.**URL**
URL 是 Web 中的一个核心概念,它是浏览器用来检索 web 上公布的任何资源的机制,一个完整的URL包括–协议部分、网址、文件地址部分

网址部分大家也经常用到，比如www.baidu.com, www.taobao.com, http://www.topbook.cc 。这是一个网站独一无二的网络名字


如果我们去掉www的前缀，比如topbook.cc,这个部分就叫域名，域名的最右边就是顶级域名,常见的比如：.com 表示商业机构，.org表示非盈利性组织。.gov表示政府机构。.edu表示教育及科研机构。用来表示国家顶级域名的比如：.cn表示中国。.us表示美国。.jp表示日本


有时候，总公司的下属分公司、或者公司下设的其他产品网站，会使用一个与域名类似的二级域名，比如腾讯网的域名是qq.com,它的子产品QQ邮箱使用的就是二级域名mail.qq.com,包括腾讯视频http://v.qq.com


从域名后第一个/到最后一个/为止，是虚拟目录部分，从最后一个/开始到？为止，是文件名部。#号后面就是锚部分，虚拟目录、文件名、锚都不是URL必须的部分，虽然一个标准的URL地址看起来很复杂，但其实我们在输入URL的时候是只要输入网址或域名就可以了

理论上说，每个有效的 URL 都指向一个唯一的资源