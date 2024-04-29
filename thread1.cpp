#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <vector>
#include <queue>
#include <future>
using namespace std;
struct Matrix
{
    vector<vector<int>> data;
};
// 线程池类
class ThreadPool
{
public:
    ThreadPool(size_t numThreads) : stop(false)
    {
        for (size_t i = 0; i < numThreads; ++i)
        {
            workers.emplace_back(
                [this]
                {
                    for (;;)
                    {
                        function<void()> task;
                        {
                            unique_lock<mutex> lock(this->queueMutex);
                            this->condition.wait(lock, [this]
                                                 { return this->stop || !this->tasks.empty(); });
                            if (this->stop && this->tasks.empty())
                            {
                                return;
                            }
                            task = move(this->tasks.front());
                            this->tasks.pop();
                        }
                        task();
                    }
                });
        }
    }
    // 添加任务
    template <class F, class... Args>
    auto addTask(F &&f, Args &&...args) -> future<decltype(f(args...))>
    {
        using return_type = decltype(f(args...));
        auto task = make_shared<packaged_task<return_type()>>(bind(forward<F>(f), forward<Args>(args)...));
        future<return_type> result = task->get_future();
        {
            lock_guard<mutex> lock(queueMutex);
            tasks.emplace([task]()
                          { (*task)(); });
        }
        condition.notify_one();
        return result;
    }
    // 停止线程池
    ~ThreadPool()
    {
        {
            lock_guard<mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (thread &worker : workers)
        {
            worker.join();
        }
    }

private:
    vector<thread> workers;
    queue<function<void()>> tasks;
    mutex queueMutex;
    condition_variable condition;
    bool stop;
};
// 矩阵乘法函数
Matrix matrixMultiply(const Matrix &matrix1, const Matrix &matrix2)
{
    Matrix result;
    // 实现矩阵乘法的逻辑
    result.data = {{matrix1.data[0][0] * matrix2.data[0][0] + matrix1.data[0][1] * matrix2.data[1][0],
                    matrix1.data[0][0] * matrix2.data[0][1] + matrix1.data[0][1] * matrix2.data[1][1]},
                   {matrix1.data[1][0] * matrix2.data[0][0] + matrix1.data[1][1] * matrix2.data[1][0],
                    matrix1.data[1][0] * matrix2.data[0][1] + matrix1.data[1][1] * matrix2.data[1][1]}};
    return result;
}
int main()
{
    ThreadPool pool(2); // 创建线程池，包含2个线程
    // 创建两个 2x2 的矩阵
    Matrix matrix1, matrix2;
    matrix1.data = {
        {1, 2},
        {3, 4}};
    matrix2.data = {
        {5, 6},
        {7, 8}};
    // 向线程池添加矩阵乘法任务
    auto result = pool.addTask(matrixMultiply, matrix1, matrix2);
    // 获取任务执行结果
    Matrix multiplicationResult = result.get();

    cout << "Result:" << endl;
    for (const auto &row : multiplicationResult.data)
    {
        for (int element : row)
            cout << element << " ";
        cout << endl;
    }
    return 0;
}