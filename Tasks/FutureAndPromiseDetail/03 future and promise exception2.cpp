#include <iostream>
#include <future>
#include <thread>
#include <string>

struct Div {

    void operator()(std::promise<int>&& intPromise, int a, int b) {
        try {
            intPromise.set_value(a/b);
        } 
        catch (...) {
            intPromise.set_exception(std::current_exception());
        }
    }

};

void executeDivision(int num, int den) {

    std::promise<int> divPromise;
    std::future<int> divResult = divPromise.get_future();

    Div div;
    std::thread divThread (div, std::move(divPromise), num, den);

    try {
        std::cout << num << " / " << den << " = " << divResult.get() << std::endl;
    } catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }

    divThread.join();
}

int main(int argc, char const *argv[])
{
    executeDivision(20, 0);
    executeDivision(20, 10);
    return 0;
}
