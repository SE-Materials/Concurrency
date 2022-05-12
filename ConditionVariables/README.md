# Condition Variables

*Condition variables enable threads to be synchronized via messages.*  

* They need the `<condition_variable>` header.
* One thread to act as a sender, and the other as the receiver of the message; the receiver waits for the notification from the sender. 
* Typical use cases for condition variables are sender-receiver or producer-consumer workflows.

* A condition variable can be the sender but also the receiver of the message.


References: [Onenote](https://onedrive.live.com/redir?resid=E57CD436CD4D5D45%21106&page=Edit&wd=target%28Concurrency.one%7C8b8f9653-79ec-8e48-8d13-fe089d0f96b6%2FCondition%20variable%7C46b64ed5-d024-4061-b827-312096d915ba%2F%29&wdorigin=703)  

### Condition variable methods

| Method	| Description |
|-----------|-------------|
| `cv.notify_one()`	| Notifies a waiting thread. |
| `cv.notify_all()`	| Notifies all waiting threads.|
| `cv.wait(lock, ...)`	| Waits for the notification while holding a `std::unique_lock`.|
| `cv.wait_for(lock, relTime, ...)`	| Waits for a time duration for the notification while holding a `std::unique_lock`.|
| `cv.wait_until(lock, absTime, ...)`	| Waits until a time point for the notification while holding a `std::unique_lock`.|


### `cv.notify_one() vs cv.notify_all()`

* The subtle difference between `cv.notify_one` and `cv.notify_all` is that `cv.notify_all` will notify all waiting threads.
* In contrast, `cv.notify_one` will notify only one of the waiting threads while the other threads remain in the wait state.

### Workflow

1. ​The program has two child threads: t1 and t2. 
2. They get their work package `waitingForWork` and `setDataRead`
3. `setDataReady` notifies - using the condition variable `condVar` - that it is done with the preparation of the work: `condVar.notify_one()`.
4. While holding the lock, thread t1 waits for its notification: `condVar.wait(lck,[]{ return dataReady; })`. 
5. *Meanwhile, the sender and receiver need a lock.*
6. In the case of the sender a `std::lock_guard` is sufficient, because it calls to lock and unlock only once.
7. In the case of the receiver, a `std::unique_lock` is necessary because it frequently locks and unlocks its mutex. The waiting thread has quite a complicated workflow.

```cpp
// conditionVariable.cpp

#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>

std::mutex mutex_;
std::condition_variable condVar;

bool dataReady{false};

void doTheWork(){
  std::cout << "Processing shared data." << std::endl;
}

void waitingForWork(){
    std::cout << "Worker: Waiting for work." << std::endl;
    std::unique_lock<std::mutex> lck(mutex_);
    condVar.wait(lck, []{ return dataReady; });
    condVar.wait(lck);
    doTheWork();
    std::cout << "Work done." << std::endl;
}

void setDataReady(){
    {
      std::lock_guard<std::mutex> lck(mutex_);
      dataReady = true;
    }
    std::cout << "Sender: Data is ready."  << std::endl;
    condVar.notify_one();
}

int main(){

  std::cout << std::endl;

  std::thread t1(waitingForWork);
  std::thread t2(setDataReady);

  t1.join();
  t2.join();

  std::cout << std::endl;
  
}
```

## Wait workflow

* If it is the first time `wait` is invoked, the following steps will happen.
    * The call to wait locks the mutex and checks if the predicate `[]{ return dataReady; }` evaluates to `true`.
        * If `true`, the condition variable unlocks the mutex and continues.
        * If `false`, the condition variable unlocks the mutex and puts itself back in the wait state.

* **Subsequent `wait` calls behave differently:**
    * The waiting thread gets a notification. It locks the mutex and checks if the predicate `[]{ return dataReady; }` evaluates to `true`.
        * If `true`, the condition variable unlocks the mutex and continues.
        * If `false`, the condition variable unlocks the mutex and puts itself back in the wait state.

* The predicate is a kind of memory for the stateless condition variable; therefore, the wait call always checks the predicate at first. 


## Lost wakeup

* The phenomenon of the lost wakeup is that the sender sends its notification before the receiver gets to a wait state. 
* The consequence is that the notification is lost. 
* The C++ standard describes condition variables as a simultaneous synchronization mechanism: 
    * “The condition_variable class is a synchronization primitive that can be used to block a thread, or multiple threads at the same time, …”. 
* So, the notification gets lost and the receiver is waiting, and waiting, and…


## Spurious wakeup

* It can happen that the receiver wakes up, although no notification happened. 
* At a minimum, POSIX Threads and the Windows API can be victims of these phenomena.

**In most of the use-cases, tasks are the less error-prone way to synchronize threads.**