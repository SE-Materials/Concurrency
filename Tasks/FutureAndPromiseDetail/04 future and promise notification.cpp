#include <iostream>
#include <thread>
#include <future>

void doTheWork() {
    std::cout << "Processing shared data " << std::endl;
}

void waitingForWork(std::future<void>&& fut) {
    std::cout << "Waiting for work ... " << std::endl;
    
    fut.get();
    doTheWork();

    std::cout << "Work done ..." << std::endl;
}

void setDataReady(std::promise<void>&& sendReady) {
    std::cout << "Sender : data is ready " << std::endl;
    sendReady.set_value();
}

int main(int argc, char const *argv[])
{
    std::promise<void> sendReady;
    auto fut = sendReady.get_future();

    std::thread t1(waitingForWork, std::move(fut));
    std::thread t2(setDataReady, std::move(sendReady));

    t1.join();
    t2.join();

    std::cout << std::endl;
    
    return 0;
}
