#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

struct CriticalData {
    std::mutex mu;
};

void deadLock(CriticalData& a, CriticalData& b) {
    
    a.mu.lock();

    std::cout << "Get the first mutex" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    b.mu.lock();
    std::cout << "Get the second mutex" << std::endl;

    // Do something with a and b

    a.mu.unlock();
    b.mu.unlock();
}

int main(int argc, char const *argv[])
{
    CriticalData a, b;
    std::thread t1(deadLock, std::ref(a), std::ref(b));
    std::thread t2(deadLock, std::ref(b), std::ref(a));

    t1.join();
    t2.join();
    return 0;
}
