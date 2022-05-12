#include <iostream>
#include <thread>
#include <mutex>


class MySingleton {

private:
    static std::once_flag initInstanceFlag;
    static MySingleton* instance;

    MySingleton() = default;
    ~MySingleton() = default;

    MySingleton(const MySingleton&) = delete;
    MySingleton& operator=(const MySingleton& ) = delete;

public:

    static MySingleton* GetInstance() {
        std::call_once(initInstanceFlag, MySingleton::initSingleton);
        return instance;
    }

    static void initSingleton() {
        instance = new MySingleton();
    }
};

MySingleton* MySingleton::instance = nullptr;
std::once_flag MySingleton::initInstanceFlag;


int main(int argc, char const *argv[])
{
    std::thread t1 ([]() {std::cout << "MySingleton::GetInstance(): "<< MySingleton::GetInstance() << std::endl;});
    std::thread t2 ([]() {std::cout << "MySingleton::GetInstance(): "<< MySingleton::GetInstance() << std::endl;});
    std::thread t3 ([]() {std::cout << "MySingleton::GetInstance(): "<< MySingleton::GetInstance() << std::endl;});

    std::cout << "MySingleton::GetInstance(): "<< MySingleton::GetInstance() << std::endl;
    std::cout << "MySingleton::GetInstance(): "<< MySingleton::GetInstance() << std::endl;

    t1.join();
    t2.join();
    t3.join();

    return 0;
}
