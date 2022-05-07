#include <iostream>
#include <thread>
#include <mutex>

std::mutex cmutex;

class Worker
{
private:
    std::string name;
public:
    Worker(std::string n) : name{n} {}
    void operator()()
    {
        for (int i=0; i<3; i++) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            cmutex.lock();
            std::cout << name << ": work " << i << " done!" << std::endl; 
            cmutex.unlock();
        }

            
    }
};

int main(int argc, char const *argv[])
{
    std::cout << "Boss: Let's start working" << std::endl;

    auto herb   = std::thread(Worker("Herb"));
    auto andrei = std::thread(Worker("  Andrei"));
    auto scott  = std::thread(Worker("      Scott"));
    auto bjarne = std::thread(Worker("          Bjarne"));
    auto bart   = std::thread(Worker("              Bart"));
    auto jenne  = std::thread(Worker("                  Jenne"));

    herb.join();
    andrei.join();
    scott.join();
    bjarne.join();
    bart.join();
    jenne.join();

    std::cout << std::endl << "Boss: Let's go home!" << std::endl;
     
    return 0;
}
