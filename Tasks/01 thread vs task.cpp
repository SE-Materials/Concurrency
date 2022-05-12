#include <iostream>
#include <thread>
#include <future>

int main(int argc, char const *argv[])
{
    int res;
    std::thread t([&]{ res = 2000 + 11;});
    t.join();

    std::cout << "res : " << res << std::endl;

    auto fut = std::async([]{ return 2000 + 11;});
    std::cout << "fut.get(): " << fut.get() << std::endl;
    
    return 0;
}
