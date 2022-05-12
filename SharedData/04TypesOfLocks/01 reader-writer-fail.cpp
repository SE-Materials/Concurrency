#include <iostream>
#include <thread>
#include <map>
#include <shared_mutex>
#include <string>

std::map<std::string, int> teleBook = {
    {"Dijkstra", 1972},
    {"Scott", 1976},
    {"Ritchie", 1983}};

std::shared_timed_mutex teleBookMutex;

void AddToTeleBook(const std::string &na, int tele)
{
    std::lock_guard<std::shared_timed_mutex> writerLock(teleBookMutex);
    std::cout << "\nStarting update " << na;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    teleBook[na] = tele;

    std::cout << " ... Ending update " << na << std::endl;
}

void PrintNumber(const std::string &na)
{
    std::shared_lock<std::shared_timed_mutex> readerLock(teleBookMutex);
    std::cout << na << ": " << teleBook[na];
}

int main(int argc, char const *argv[])
{
    std::cout << std::endl;

    std::thread reader1([] { PrintNumber("Scott"); });
    std::thread reader2([] { PrintNumber("Ritchie"); });
    std::thread w1([] { AddToTeleBook("Scott", 1968); });
    std::thread reader3([] { PrintNumber("Dijkstra"); });
    std::thread reader4([] { PrintNumber("Scott"); });
    std::thread w2([] { AddToTeleBook("Bjarne", 1965); });
    std::thread reader5([] { PrintNumber("Scott"); });
    std::thread reader6([] { PrintNumber("Ritchie"); });
    std::thread reader7([] { PrintNumber("Scott"); });
    std::thread reader8([] { PrintNumber("Bjarne"); });

    reader1.join();
    reader2.join();
    reader3.join();
    reader4.join();
    reader5.join();
    reader6.join();
    reader7.join();
    reader8.join();
    w1.join();
    w2.join();

    std::cout << std::endl;

    std::cout << "\nThe new telephone book" << std::endl;
    for (auto teleIt : teleBook)
    {
        std::cout << teleIt.first << ": " << teleIt.second << std::endl;
    }

    std::cout << std::endl;
    return 0;
}
