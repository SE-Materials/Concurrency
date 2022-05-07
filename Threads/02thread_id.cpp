#include <iostream>
#include <thread>

int main()
{
    std::cout << "Master : " << std::this_thread::get_id() << std::endl;
    std::thread t([]{std::cout << "Child : " << std::this_thread::get_id() << std::endl;});
    
    std::cout << "Child joinable? " << std::boolalpha << t.joinable() << std::endl;
    t.join();
    std::cout << "Child joinable? " << std::boolalpha << t.joinable() << std::endl;
    return 0;
}