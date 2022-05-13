#include <iostream>
#include <future>

void product(std::promise<int>&& intPromise, int a, int b) {
    intPromise.set_value(a * b);
}

struct Div {

    void operator()(std::promise<int>&& intPromise, int a, int b) {
        intPromise.set_value(a / b);
    }
};

int main(int argc, char const *argv[])
{
    int a {20};
    int b {10};

    std::promise<int> productPromise;
    std::promise<int> multiplyPromise;

    // Get the futures from the promises
    std::future<int> productResult  = productPromise.get_future();
    std::future<int> multiplyResult = multiplyPromise.get_future();

    // Calculate results in separate thread.
    std::thread prodThread(product, std::move(productPromise), a, b);

    Div div;
    std::thread multThread(div, std::move(multiplyPromise), a, b);

    // Get the result
    std::cout << "20 * 10 = " << productResult.get() << std::endl;
    std::cout << "20 / 10 = " << multiplyResult.get() << std::endl;

    prodThread.join();
    multThread.join();

    return 0;
}
