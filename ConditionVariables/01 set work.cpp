#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mu;
std::condition_variable cv;

bool dataReady {false};

void doTheWork() {
    std::cout << "Processing shared data " << std::endl;
}

void waitingForWork() {
    std::cout << "Waiting for work ... " << std::endl;
    
    std::unique_lock<std::mutex> lk(mu);

    cv.wait(lk, [] { return dataReady; });
    cv.wait(lk);

    doTheWork();

    std::cout << "Work done ..." << std::endl;
}

void setDataReady() {
    {
        std::lock_guard<std::mutex> lk (mu);
        dataReady = true;
    }
    std::cout << "Sender : data is ready " << std::endl;
    cv.notify_one();
}

int main(int argc, char const *argv[])
{
    std::thread t1(waitingForWork);
    std::thread t2(setDataReady);

    t1.join();
    t2.join();

    std::cout << std::endl;
    
    return 0;
}
