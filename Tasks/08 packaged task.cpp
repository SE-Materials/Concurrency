#include <iostream>
#include <future>
#include <deque>

int factorial(int N)
{
    int res = 1;
    for (int i = N; i >= 1; i--)
        res *= i;

    std::cout << "Result is : " << res << std::endl;
    return res;
}

std::deque<std::packaged_task<int()>> task_q;
std::mutex mu;

void thread_1()
{
    std::packaged_task<int()> t;
    {
        std::lock_guard<std::mutex> locker(mu);
        t = std::move(task_q.front());
        task_q.pop_front();
    }

    t();
}

int main()
{
    std::packaged_task<int()> t(std::bind(factorial, 6));
    std::future<int> fu = t.get_future();

    {
        std::lock_guard<std::mutex> locker(mu);
        
        // I will not execute here. Hoping that somebody will pop up the task and execute.
        // But it has some return value.. how do we get that ? - Using future.
        task_q.push_back(std::move(t));
    }

    std::thread t1(thread_1);

    std::cout << fu.get() << std::endl;
    t1.join();
}