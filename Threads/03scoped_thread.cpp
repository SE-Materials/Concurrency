#include <iostream>
#include <thread>

class scoped_thread
{
    std::thread t;
public:

    // copy constructor of thread is deleted
    // pass by value, so that user does not create and pass from somewhere else
    scoped_thread(std::thread t_) : t{std::move(t_)} 
    {
        if (!t.joinable())
        {
            throw std::logic_error("No thread");
        }
    }

    scoped_thread& operator=(const scoped_thread& other) = delete;
    scoped_thread(const scoped_thread& other) = delete;

    ~scoped_thread()
    {
        t.join();
    }
};

int main(int argc, char const *argv[])
{
    scoped_thread (std::thread([]{std::cout << "Hi from scoped thread" << std::endl;}));

    // Not possible
    // std::thread t1([]{std::cout << "Hi from scoped thread" << std::endl;});
    // scoped_thread st(t1);

    return 0;
}

