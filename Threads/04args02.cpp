#include <iostream>
#include <thread>
#include <chrono>

class Sleeper
{
    int& i;

    public:
    Sleeper(int& i) : i{i}
    {

    }

    void operator()(int k)
    {
        for (unsigned int j=0; j<=5; j++)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            i += k;
        }
        std::cout << std::this_thread::get_id() << std::endl;
    }
};

// Not working
void fn1()
{
    int varSleeper = 100;
    std::thread t1(Sleeper(varSleeper), 5);

    // t1.detach() // not working
    t1.join(); // working
    std::cout << "var sleeper : " << varSleeper << std::endl;
}


int main(int argc, char const *argv[])
{
    fn1();
    return 0;
}
