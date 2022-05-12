#include <iostream>
#include <mutex>
#include <thread>

std::once_flag onceFlag;

void doOnce() {
    std::call_once(onceFlag, []() {std::cout << "only once." << std::endl;});
}

int main(int argc, char const *argv[])
{
    std::thread t1(doOnce);
    std::thread t2(doOnce);
    std::thread t3(doOnce);
    std::thread t4(doOnce);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}
