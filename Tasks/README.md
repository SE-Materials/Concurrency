# Tasks

*C++ has tasks to perform work asynchronously.*

* Need `<future>` header.
* Two associated components: `promise` and `future`.
    * Both of them are connected via a data channel.
* The `promise` executes the work package and puts the result in the data channel.
* The associated `future` picks up the result.
* Both communication endpoints can run in separate threads.
* Future can pick up the results at a later time.


References:
* [Onenote](https://onedrive.live.com/redir?resid=E57CD436CD4D5D45%21106&page=Edit&wd=target%28Concurrency.one%7C8b8f9653-79ec-8e48-8d13-fe089d0f96b6%2FFuture%2C%20promise%20and%20async%7C772599a9-14e7-4fe2-8680-8a6338826663%2F%29&wdorigin=703)
* [Onenote](https://onedrive.live.com/redir?resid=E57CD436CD4D5D45%21106&page=Edit&wd=target%28Concurrency.one%7C8b8f9653-79ec-8e48-8d13-fe089d0f96b6%2Fpackaged%20task%7Cb2f832ab-1c34-964a-bb64-2de56960f13b%2F%29&wdorigin=703)


## Threads vs Tasks

* `thread` : Print from new thread
* `fut.get()` : call is blocking. Creates `promise` and `future`. 

```cpp
#include <iostream>
#include <thread>
#include <future>

int main(int argc, char const *argv[])
{
    int res;
    std::thread t([&]{ res = 2000 + 11;});
    t.join();

    std::cout << "res : " << res << std::endl;

    auto fut = std::async([]{ return 2000 + 11;});
    std::cout << "fut.get(): " << fut.get() << std::endl;
    
    return 0;
}
```


| Criteria	                |                 Threads            	|        Tasks                         |
|:-------------------------:|:-------------------------------------:|:------------------------------------:|
| Participants              |	creator and child thread            | promise and future                   |
| Communication	            | shared variable	                    | communication channel                |
| Thread creation           |	obligatory	                        | optional                             |
| Synchronisation	        | via `join()` (waits)	                | get call blocks                      |
| Exception in child thread	| child and creator threads terminates	| return value of the promise          |
| Kinds of communication    |	values	                            | values, notifications, and exceptions|

* The `future` can request the result of the task only once (by calling fut.get()). 
* Calling it more than once results in undefined behavior. 
* This is not true for a `std::shared_future`, which can be queried multiple times.

* A promise can serve one or many futures. 
* A promise can send a value, an exception, or just a notification. 
* In addition, you can use a safe replacement for a condition variable.
* `std::async` is the easiest way to create a future and we’ll see why in the next lesson.

---
### Three ways to get `std::future`

* `promise::get_future()`
* `packaged_task::get_future()`
* `async()` returns a `future`
---

## `std::async`

* `std::async` behaves like an asynchronous function call.
* This function call takes a callable together with its arguments.
* `std::async` is a variadic template and can, therefore, take an arbitrary number of arguments. 
* The call to std::async returns a `future` object fut. That’s your handle for getting the result via `fut.get()`.

---
### std::async should be your first choice

* The C++ runtime decides if `std::async` is executed in a separate thread. 
* The decision of the C++ runtime may depend on the number of CPU cores available, the utilization of your system, or the size of your work package. 
* By using `std::async`, you only specify the task that should run; the C++ runtime automatically manages the creation and also the lifetime of the thread.
---


### async: Start Policy

* With the start policy you can explicitly specify whether the asynchronous call should be executed in the same thread (`std::launch::deferred`) or in another thread (`std::launch::async`).

#### It has `Eager vs Lazy` evaluation 

* By default, `std::async` executes its work package immediately. This is why it’s called eager evaluation.
* `auto fut= std::async(std::launch::deferred, ... )`; the promise will not be executed immediately. The call `fut.get()` starts the promise lazily; i.e. the promise will only run if the future asks via `fut.get()` for the result.

```cpp
// asyncLazy.cpp

#include <chrono>
#include <future>
#include <iostream>

int main(){

  std::cout << std::endl;

  auto begin= std::chrono::system_clock::now();

  auto asyncLazy=std::async(std::launch::deferred,
                            []{ return std::chrono::system_clock::now(); });

  auto asyncEager=std::async(std::launch::async,
                             []{ return std::chrono::system_clock::now(); });

  std::this_thread::sleep_for(std::chrono::seconds(1));

  auto lazyStart= asyncLazy.get() - begin;
  auto eagerStart= asyncEager.get() - begin;

  auto lazyDuration= std::chrono::duration<double>(lazyStart).count();
  auto eagerDuration=  std::chrono::duration<double>(eagerStart).count();

  std::cout << "asyncLazy evaluated after : " << lazyDuration 
            << " seconds." << std::endl;
  std::cout << "asyncEager evaluated after: " << eagerDuration 
            << " seconds." << std::endl;

  std::cout << std::endl;

}
```

### async: Fire and Forget

* Fire and forget futures are special `futures`.
* They execute just in place because their future is not bound to a variable. 
* For a “fire and forget” `future`, it is necessary that the promise runs in a separate thread so it can immediately start its work. 
* This is done by the `std::launch::async` policy.

```cpp
#include<iostream>
#include<future>

int main(){

    auto fut = std::async([]{ return 2011; });
    std::cout << fut.get() << std::endl;         
    
    std::async(std::launch::async, 
            []{ std::cout << "fire and forget" << std::endl; });
}
```

* Fire and forget `futures` look very promising but have a big drawback. 
* A `future` that is created by `std::async` waits on its destructor, until its promise is done.
* In this context, waiting is not very different from blocking. The `future` blocks the progress of the program in its destructor. This becomes more obvious when you use fire and forget futures. What seems to be concurrent actually runs sequentially.



```cpp
// fireAndForgetFutures.cpp

#include <chrono>
#include <future>
#include <iostream>
#include <thread>

int main(){

  std::cout << std::endl;

  std::async(std::launch::async, []{
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "first thread" << std::endl;
    });
    
  std::async(std::launch::async, []{
    std::this_thread::sleep_for(std::chrono::seconds(1));  
    std::cout << "second thread" << std::endl;}
    );
  
  std::cout << "main thread" << std::endl;  

  std::cout << std::endl;

}
```

* The result is that the promises will be executed in the sequence which you find in the source code.
* That being said, the execution sequence is independent of the execution time;


### async: Concurrent calculation

```cpp
// dotProductAsync.cpp

#include <iostream>
#include <future>
#include <random>
#include <vector>
#include <numeric>

using namespace std;

static const int NUM= 100000000;

long long getDotProduct(vector<int>& v, vector<int>& w){
  
  auto vSize = v.size();

  auto future1 = async([&]{ 
    return inner_product(&v[0], &v[vSize/4], &w[0], 0LL); 
  });
  
  auto future2 = async([&]{ 
    return inner_product(&v[vSize/4], &v[vSize/2], &w[vSize/4], 0LL); 
  });
  
  auto future3 = async([&]{ 
    return inner_product(&v[vSize/2], &v[vSize* 3/4], &w[vSize/2], 0LL);
  });
  
  auto future4 = async([&]{ 
    return inner_product(&v[vSize * 3/4], &v[vSize], &w[vSize * 3/4], 0LL); 
  });

  return future1.get() + future2.get() + future3.get() + future4.get();
}


int main(){

  cout << endl;

  random_device seed;

  // generator
  mt19937 engine(seed());

  // distribution
  uniform_int_distribution<int> dist(0, 100);

  // fill the vectors
  vector<int> v, w;
  v.reserve(NUM);
  w.reserve(NUM);
  for (int i=0; i< NUM; ++i){
    v.push_back(dist(engine));
    w.push_back(dist(engine));
  }

  cout << "getDotProduct(v, w): " << getDotProduct(v, w) << endl;

  cout << endl;

}
```



## `std::packaged_task`

Dealing with `std::packaged_task` usually consists of four steps:

1. Wrap your work:
    `std::packaged_work<int(int, int)> sumTask([](int a, int b) { return a + b; } );`
<br>
2. Create a `future`:
    `std::future<int> sumResult = sumTask.get_future();`
<br>
3. Perform the calcuation : 
    `sumTask(2000, 11);`
<br>
4. Query the result : 
    `sumResult.get();`
<br>


```cpp
// packagedTask.cpp

#include <utility>
#include <future>
#include <iostream>
#include <thread>
#include <deque>

class SumUp{
  public:
    int operator()(int beg, int end){
      long long int sum{0};
      for (int i = beg; i < end; ++i ) sum += i;
      return sum;
    }
};

int main(){

  std::cout << std::endl;

  SumUp sumUp1;
  SumUp sumUp2;
  SumUp sumUp3;
  SumUp sumUp4;

  // wrap the tasks
  std::packaged_task<int(int, int)> sumTask1(sumUp1);
  std::packaged_task<int(int, int)> sumTask2(sumUp2);
  std::packaged_task<int(int, int)> sumTask3(sumUp3);
  std::packaged_task<int(int, int)> sumTask4(sumUp4);

  // create the futures
  std::future<int> sumResult1 = sumTask1.get_future();
  //std::future<int> sumResult2 = sumTask2.get_future();
  auto sumResult2 = sumTask2.get_future();
  std::future<int> sumResult3 = sumTask3.get_future();
  //std::future<int> sumResult4 = sumTask4.get_future();
  auto sumResult4 = sumTask4.get_future();

  // push the tasks on the container
  std::deque<std::packaged_task<int(int,int)>> allTasks;
  allTasks.push_back(std::move(sumTask1));
  allTasks.push_back(std::move(sumTask2));
  allTasks.push_back(std::move(sumTask3));
  allTasks.push_back(std::move(sumTask4));
  
  int begin{1};
  int increment{2500};
  int end = begin + increment;

  // perform each calculation in a separate thread
  while (not allTasks.empty()){
    std::packaged_task<int(int, int)> myTask = std::move(allTasks.front());
    allTasks.pop_front();
    std::thread sumThread(std::move(myTask), begin, end);
    begin = end;
    end += increment;
    sumThread.detach();
  }

  // pick up the results
  auto sum = sumResult1.get() + sumResult2.get() + 
             sumResult3.get() + sumResult4.get();

  std::cout << "sum of 0 .. 10000 = " << sum << std::endl;

  std::cout << std::endl;

}
```

### Explanation:

1. Wrap the tasks: I pack the work packages in `std::packaged_task` (lines 28 - 31) objects. 
    * Work packages are instances of the class SumUp (lines 9 - 16). 
    * The work is done in the call operator (lines 11 - 15) which sums up all numbers from beg to end -1 and returns the sum as the result. 
    * `std::packaged_task` (lines 28 - 31) can deal with callables that need two ints and return an `int: int(int, int)`.
<br>
2. Create the `futures`: I have to create the `future` objects with the help of `std::packaged_task` objects (lines 34 to 3). 
    * The packaged_task is the promise in the communication channel. 
    * The type of the `future` is defined explicitly as `std::future<int> sumResult1= sumTask1.get_future()`, but the compiler can do that job for me with `auto sumResult1= sumTask1.get_future()`.
<br>

3. Perform the calculations: Now the calculation takes place. 
    * First, the packaged_task is moved onto the `std::deque` (lines 43 - 46), then each `packaged_task` (lines 54 - 59) is executed in the while loop. 
    * To do that, I move the head of the `std::deque` in an `std::packaged_task` (line 54), move the `packaged_task` in a new `thread` (line 56) and let it run in the background (line 59). 
    * I used the move semantic in lines 54 and 56 because `std::packaged_task` objects are not copyable. 
    * This restriction holds for all promises, but also for futures and threads. However, there is one exception to this rule: `std:.shared_future`.
<br>

4. Pick up the results: In the final step I ask all futures for their value and sum them up (line 63).
<br>

The class templates `std::promise` and `std::future` provide you the full control over tasks.