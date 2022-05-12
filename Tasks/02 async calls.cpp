#include <iostream>
#include <future>
#include <chrono>



int main(int argc, char const *argv[])
{
    auto begin = std::chrono::system_clock::now();

    auto asyncLazyFut = std::async(std::launch::deferred, []{
        return std::chrono::system_clock::now();
    });

    // auto asyncEagerFut = std::async([]{ return std::chrono::system_clock::now(); });

    auto asyncEagerFut = std::async(std::launch::async, []{
        return std::chrono::system_clock::now();
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto lazyStart  = asyncLazyFut.get() - begin;
    auto eagerStart = asyncEagerFut.get() - begin;

    auto lazyDuration = std::chrono::duration<double>(lazyStart).count();
    auto eagerDuration = std::chrono::duration<double>(eagerStart).count();

    std::cout << "Async lazy evaluated after : " << lazyDuration << " seconds." << std::endl;
    std::cout << "Async eager evaluated after : " << eagerDuration << " seconds." << std::endl;

    return 0;
}
