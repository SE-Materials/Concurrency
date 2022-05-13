# Promises and Futures

*A promise can put a value, an exception, or simply a notification into the shared data channel. One promise can serve many `std::shared_future` futures.*

## Basic Usage

```cpp
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
```

## `std::promise`

* `std::promise` enables you to set a value, a notification, or an exception.  
* In addition, the promise can provide its result in a delayed fashion.


| Method	                                      | Description         |
|:-----------------------------------------------:|:-------------------:|
| `prom.swap(prom2)` and `std::swap(prom, prom2)` | Swaps the promises.	|
| `prom.get_future()`                             |	Returns the future. |
| `prom.set_value(val)`                           |	Sets the value.     |
| `prom.set_exception(ex)`                        |	Sets the exception. |
| `prom.set_value_at_thread_exit(val)`            |	Stores the value and makes it ready if the promise exits. |
| `prom.set_exception_at_thread_exit(ex)`         |	Stores the exception and makes it ready if the promise exits. |


---
### Note:
If the value or the exception is set by the promise more then once, a `std::future_error` exception is thrown.

---

## `std::future`

* It picks up the value from the promise.
* Asks the promise if the value is available.
* Wait for the notification of the promise. 
* This waiting can be done with a relative time duration or an absolute time point.
* Create a shared future (`std::shared_future`).

| Method	                |             Description                                                          |
|:-------------------------:|---------------------------------------------------------------------------------:|
| `fut.share()`	            | Returns a `std::shared_future`. Afterwards, the result is not available anymore. |
| `fut.get()`	            | Returns the result which can be a value or an exception. |
| `fut.valid()`	            | Checks if the result is available. After calling `fut.get()` it returns false. |
| `fut.wait()`              | Waits for the result. |
| `fut.wait_for(relTime)`	| Waits for the result, but not longer than for a relTime. |
| `fut.wait_until(absTime)` | Waits for the result, but not longer than until abstime. |

---
### Note:
If a future `fut` asks for the result more than once, a `std::future_error` exception is thrown.

---

* There is a one-to-one relationship between the promise and the future. 
* In contrast, std::shared_future supports one-to-many relations between a promise and many futures.


## Promise and Future: Return an Exception

* The promise deals with the issue that the denominator is 0. If the denominator is 0, it sets the exception.
* Following that, the future has to deal with the exception in its try-catch block (lines 33 - 38).
* **If possible, use tasks as a safe replacement of condition variables.**

```cpp
#include <iostream>
#include <future>
#include <thread>
#include <string>

struct Div {

    void operator()(std::promise<int>&& intPromise, int a, int b) {
        try {
            if (b == 0) {
                std::string errMessage = std::string("Illegal division by 0: ") + std::to_string(a) + " / " + std::to_string(b);
                throw std::runtime_error(errMessage);
            }

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
    } catch (std::runtime_error &e) {
        std::cout << e.what() << std::endl;
    }

    divThread.join();
}

int main(int argc, char const *argv[])
{
    executeDivision(20, 0);
    executeDivision(20, 10);
    return 0;
}
```

## Promise and future: Notification

*If you use promises and futures to synchronize threads, they have a lot in common with condition variables.*

* Most of the time, promises and futures are the better choices.

| Criteria	                 | Condition Variables	|Tasks |
|:--------------------------:|:--------------------:|:----:| 
| Multiple synchronizations	 | Yes	                | No |
| Critical section	         | Yes	                | No |
| Error handling in receiver |	No	                | Yes |
| Spurious wakeup	         | Yes	                | No |
| Lost wakeup	             | Yes	                | No |

* The advantage of a condition variable to a promise and future is that you can use condition variables to synchronize threads multiple times. 
* In contrast to that, a promise can send its notification only once, so you have to use more promise and future pairs to get the functionality of a condition variable. 


* If you use the condition variable for only one synchronization, the condition variable is a lot more difficult to use in the right way.
* A promise and future pair needs no shared variable and, therefore, it doesn’t have a lock, and isn’t prone to spurious or lost wakeups. 
* In addition to that, tasks can handle exceptions. 

```cpp
#include <iostream>
#include <thread>
#include <future>

void doTheWork() {
    std::cout << "Processing shared data " << std::endl;
}

void waitingForWork(std::future<void>&& fut) {
    std::cout << "Waiting for work ... " << std::endl;
    
    fut.get();
    doTheWork();

    std::cout << "Work done ..." << std::endl;
}

void setDataReady(std::promise<void>&& sendReady) {
    std::cout << "Sender : data is ready " << std::endl;
    sendReady.set_value();
}

int main(int argc, char const *argv[])
{
    std::promise<void> sendReady;
    auto fut = sendReady.get_future();

    std::thread t1(waitingForWork, std::move(fut));
    std::thread t2(setDataReady, std::move(sendReady));

    t1.join();
    t2.join();

    std::cout << std::endl;
    
    return 0;
}
```


## `std::shared_future`

* The future creates a shared future by using `fut.share()`. 
* Shared future is associated with its promise and can independently ask for the result. 
* A `std::shared_future` has the same interface as a `std::future`.


* In addition to the `std::future`, a `std::shared_future` enables you to query the promise independently of the other associated futures.


### 2 ways to create `std::shared_future` ?

1. Invoke `fut.share()` on a `std::future` fut.
    * Afterwards, the result in no longer available. That means valid == false

2. Initialize a `std::shared_future` from a `std::promise` : `std::shared_future<int> divResult= divPromise.get_future()`


---
#### Note:
The handling of a `std::shared_future` is special.

---

```cpp
// sharedFuture.cpp

#include <future>
#include <iostream>
#include <thread>
#include <utility>

std::mutex coutMutex;

struct Div{

  void operator()(std::promise<int>&& intPromise, int a, int b){
    intPromise.set_value(a/b);
  }

};

struct Requestor{

  void operator ()(std::shared_future<int> shaFut){

    // lock std::cout
    std::lock_guard<std::mutex> coutGuard(coutMutex);

    // get the thread id
    std::cout << "threadId(" << std::this_thread::get_id() << "): " ;
    
    std::cout << "20/10= " << shaFut.get() << std::endl;

  }

};

int main(){

  std::cout << std::endl;

  // define the promises
  std::promise<int> divPromise;

  // get the futures
  std::shared_future<int> divResult = divPromise.get_future();

  // calculate the result in a separat thread
  Div div;
  std::thread divThread(div, std::move(divPromise), 20, 10);

  Requestor req;
  std::thread sharedThread1(req, divResult);
  std::thread sharedThread2(req, divResult);
  std::thread sharedThread3(req, divResult);
  std::thread sharedThread4(req, divResult);
  std::thread sharedThread5(req, divResult);

  divThread.join();

  sharedThread1.join();
  sharedThread2.join();
  sharedThread3.join();
  sharedThread4.join();
  sharedThread5.join();

  std::cout << std::endl;

}
```

---
### Note:

In contrast to an `std::future` object that can only be moved, you can copy an `std::shared_future` object

---