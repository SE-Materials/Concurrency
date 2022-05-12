#include <iostream>
#include <thread>


class ScopedThread {

    std::thread t;
public:

    ScopedThread(std::thread inputThread) : t{std::move(inputThread)} {
        if (!t.joinable())
        {
            throw std::logic_error("No thread");
        }
    }

    ScopedThread(const ScopedThread& ) = delete;
    ScopedThread& operator=(const ScopedThread& ) = delete;

    ~ScopedThread() {
        t.join();
    }
};

int main(int argc, char const *argv[])
{
    ScopedThread th(std::thread([](){ std::cout << "Hi!!" << std::endl;}));
    return 0;
}
