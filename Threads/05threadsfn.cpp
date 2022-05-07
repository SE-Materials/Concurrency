#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char const *argv[])
{
    std::cout << "Concurrency: " << std::thread::hardware_concurrency() << std::endl;

    std::thread t1([](){std::cout << "T1: " << std::this_thread::get_id() << std::endl;});
    std::thread t2([](){std::cout << "T2: " << std::this_thread::get_id() << std::endl;});

    std::cout << std::endl;

    std::cout << "From main, T1: " << t1.get_id() << std::endl; 
    std::cout << "From main, T2: " << t2.get_id() << std::endl; 

    std::swap(t1, t2); // or t1.swapt(t2);

    std::cout << "From main, T1: " << t1.get_id() << std::endl; 
    std::cout << "From main, T2: " << t2.get_id() << std::endl;

    t1.join();
    t2.join();
     
    return 0;
}
