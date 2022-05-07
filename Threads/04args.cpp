#include <iostream>
#include <thread>

//------------------------------------------------------------------------
void arg1()
{
     std::string s{"c++11"};

    std::thread t1([=]{ std::cout << s << std::endl;});

    std::thread t2([&]{ std::cout << s << std::endl;});

    t1.join();
    t2.join();
}

//------------------------------------------------------------------------
class Account
{

};

void transferMoney(int amount, Account& from, Account& to)
{
    std::cout << "Starting money tranfer " << std::endl;
}

void arg2()
{
    Account account1, account2;
    std::thread t(transferMoney, 10, std::ref(account1), std::ref(account2));
    t.join();
}

//------------------------------------------------------------------------
int main(int argc, char const *argv[])
{
    arg1();
    arg2();

    return 0;
}
