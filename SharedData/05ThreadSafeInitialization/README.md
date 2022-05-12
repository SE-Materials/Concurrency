# Thread safe initialization

*If the variable is never modified, there is no need for synchronization by using an expensive lock or an atomic. You only have to ensure that it is initialized in a thead safe way.*

Three ways to initialize variables in a thread safe way in C++:
1. Constant Expressions
2. The function `std::call_once` in combination with the flag `std::once_flag`.
3. A static variable with block scope.
4. In main thread

---
**Thread-safe initialisation in the main-thread**

The easiest and fourth way to initialise a variable in a thread-safe way: initialise the variable in the main-thread before you create any child threads.

---

## 1. Using Constant Expressions

* Constant expressions are expressions that the compiler can evaluate at compile time; they are implicitly thread-safe.
* Placing the keyword constexpr in front of a variable makes the variable a constant expression. 
* The constant expression must be initialized immediately.

```cpp
constexpr double pi = 3.14;
```

User defined types can also be constant expressions. But few restrictions : 
* They must not have virtual methods or a virtual base class.
* Their constructor must be empty and itself be a constant expression.
* Their methods, should be callable at compile time, must be constant expressions.

`MyDouble` satisfies all the above constraints. 

```cpp
// constexpr.cpp

#include <iostream>

class MyDouble{
  private:
    double myVal1;
    double myVal2;
  public:
    constexpr MyDouble(double v1,double v2):myVal1(v1),myVal2(v2){} // since callable at compile time; empty constructor
    constexpr double getSum() const { return myVal1 + myVal2; } // since callable at compile time
    double getDiff() const { return myVal1 - myVal2; }
};

int main() {

  constexpr double myStatVal = 2.0;
  constexpr MyDouble myStatic(10.5, myStatVal);
  constexpr double sumStat= myStatic.getSum();
  std::cout << "SumStat: "<<sumStat << std::endl;

  double diffStat = myStatic.getDiff();
  std::cout << "DiffStat: " << diffStat << std::endl;
}
```

## 2. Using `call_once` and `once_flag`

Both of them are present in `<mutex>`

* By using the `std::call_once` function you can register a callable. 
* The `std::once_flag` ensures that only one registered function will be invoked, but you can register additional functions via the same std::once_flag. That being said, only one function from that group is called.

`std::call_once` obeys the following rules:

* Exactly **one execution** of exactly **one of the functions** is performed. 
* It is undefined which function will be selected for execution. 
* The selected function runs in the same thread as the std::call_once invocation it was passed to.
* No invocation in the group returns before the above-mentioned execution of the selected function completes successfully.
* *If the selected function exits via an exception, it is propagated to the caller. Another function is then selected and executed.*

```cpp
#include <iostream>
#include <mutex>
#include <thread>

std::once_flag onceFlag;

void doOnce() {
    std::call_once(onceFlag, []() {std::cout << "only once." << std::endl;});
}

int main(int argc, char const *argv[])
{
    std::thread t1(doOnce);
    std::thread t2(doOnce);
    std::thread t3(doOnce);
    std::thread t4(doOnce);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}
```

### Thread safe singleton

```cpp
#include <iostream>
#include <thread>
#include <mutex>


class MySingleton {

private:
    static std::once_flag initInstanceFlag;
    static MySingleton* instance;

    MySingleton() = default;
    ~MySingleton() = default;

    MySingleton(const MySingleton&) = delete;
    MySingleton& operator=(const MySingleton& ) = delete;

public:

    static MySingleton* GetInstance() {
        std::call_once(initInstanceFlag, MySingleton::initSingleton);
        return instance;
    }

    static void initSingleton() {
        instance = new MySingleton();
    }
};

MySingleton* MySingleton::instance = nullptr;
std::once_flag MySingleton::initInstanceFlag;


int main(int argc, char const *argv[])
{
    std::thread t1 ([]() {std::cout << "MySingleton::GetInstance(): "<< MySingleton::GetInstance() << std::endl;});
    std::thread t2 ([]() {std::cout << "MySingleton::GetInstance(): "<< MySingleton::GetInstance() << std::endl;});
    std::thread t3 ([]() {std::cout << "MySingleton::GetInstance(): "<< MySingleton::GetInstance() << std::endl;});

    std::cout << "MySingleton::GetInstance(): "<< MySingleton::GetInstance() << std::endl;
    std::cout << "MySingleton::GetInstance(): "<< MySingleton::GetInstance() << std::endl;

    t1.join();
    t2.join();
    t3.join();

    return 0;
}
```


## 3. Using static variables with Block scope

* Static variables with block scope will be created exactly once and lazily (i.e. created just at the moment of the usage). 
* This characteristic is the basis of the so-called Meyers Singleton, named after Scott Meyers.
* With C++11, static variables with block scope have an additional guarantee; they will be initialized in a thread-safe way.

```cpp
// meyersSingleton.cpp
#include <iostream>
#include <thread>

class MySingleton{
public:
  static MySingleton& GetInstance(){
    static MySingleton instance;
    return instance;
  }
private:
  MySingleton() = default;
  ~MySingleton() = default;
  MySingleton(const MySingleton&)= delete;
  MySingleton& operator=(const MySingleton&)= delete;

};

int main(){

  MySingleton::GetInstance();

return 0;

}
```
