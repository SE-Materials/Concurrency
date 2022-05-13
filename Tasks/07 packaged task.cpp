#include <iostream>
#include <future>

int factorial(int N) {
    int res = 1;
    for (int i=N; i>=1; i--)
        res *= i;
    
    std::cout << "Result is : " << res << std::endl;
    return res;
}

int main(int argc, char const *argv[])
{
    std::packaged_task<int(int)> t(factorial);

    // -- works --
    // t(6); // = 720

    // -- Does not work --
    // auto res = t(6); // Can not get the value, as it returns void

    // -- works -- 
    auto factorialResult = t.get_future();
    t(6);
    std::cout << factorialResult.get() << std::endl;

    return 0;
}
