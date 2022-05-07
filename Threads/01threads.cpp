#include <iostream>
#include <thread>

// https://www.40tude.fr/how-to-compile-cpp-code-with-vscode-cl/

// 1. Callable type function
void helloFunction() 
{
    std::cout << "Hello from function" << std::endl;
}

// 2. Callable type function object
class HelloClass
{
    public:
        void operator()()
        {
            std::cout << "Hello from class" << std::endl;
        }
};

int main() 
{
    std::thread t1(helloFunction);

    auto h = HelloClass();
    std::thread t2(h);

    // 3. callable type labmda
    std::thread t3([](){std::cout << "Hello from lambda" << std::endl;});

    t1.join();
    t2.join();
    t3.join();

    return 0;
}