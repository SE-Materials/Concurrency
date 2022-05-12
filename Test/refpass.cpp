#include <iostream>
#include <thread>

class Account{
    int balance;
public:
    Account(int balance) : balance {balance} {}
    
    void WithDraw(int amount) {
        balance -= amount;
    }

    void Deposit(int amount) {
        balance += amount;
    }

    int GetBalance() {
        return balance;
    }
};

void transferAmount(int amount, Account& from, Account& to) {
    from.WithDraw(amount);
    to.Deposit(amount);
}

int main(int argc, char const *argv[])
{
    Account depositor(100);
    Account reciever(200);

    std::thread t(transferAmount, 50, std::ref(depositor), std::ref(reciever));
    t.join();

    std::cout << depositor.GetBalance() << " , " << reciever.GetBalance() << std::endl;
    return 0;
}
