# Types of locks

## 1. `std::lock_guard`

Following code can go in deadlock. Or if exception lock will never get released.
```cpp
std::mutex m;
m.lock();
sharedVariable = getVar();
m.unlock();
```

### Automatic lock release using `std::lock_guard`

Once lockGuard goes out of scope, it will unlock in it's destructor.
```cpp
{
    std::mutex m;
    std::lock_guard<std::mutex> lockGuard(m);

    sharedVariable = getVar();
}
```

### Solution to deadlock

```cpp
std::lock(a.mut, b.mut);
std::lock_guard<std::mutex> guard1(a.mut, std::adopt_lock);
std::lock_guard<std::mutex> guard2(b.mut, std::adopt_lock);
```

```cpp
void deadLock(CriticalData& a, CriticalData& b){

  lock_guard<std::mutex> guard1(a.mut, std::adopt_lock);
  cout << "Thread: " << this_thread::get_id() << " first mutex" << endl;

  this_thread::sleep_for(chrono::milliseconds(1));

  lock_guard<std::mutex> guard2(b.mut, std::adopt_lock);
  cout << "  Thread: " << this_thread::get_id() << " second mutex" <<  endl;

  cout << "    Thread: " << this_thread::get_id() << " get both mutex" << endl;
  lock(a.mut, b.mut);
  // do something with a and b
}
```


---
### Note:

`std::scoped_lock with C++17`

* With C++17 we get a `std::scoped_lock`. It’s very similar to `std::unique_lock`, but `std::scoped_lock` can lock an arbitrary number of mutexes atomically. That being said, you have to keep two facts in mind.

* If one of the current threads already owns the corresponding mutex and the mutex is not recursive, the behaviour is undefined.

* You can only take the ownership of the mutex without locking them. In this case, you have to provide the `std::adopt_lock_t` flag to the constructor: `std::scoped_lock(std::adopt_lock_t, MutexTypes& ... m)`.

* You can quite elegantly solve the previous deadlock by using a `std::scoped_lock`. I will discuss the resolution of the deadlock in the following section.
---

## 2. `std::unique_lock`

[Onenote](https://onedrive.live.com/redir?resid=E57CD436CD4D5D45%21106&page=Edit&wd=target%28Concurrency.one%7C8b8f9653-79ec-8e48-8d13-fe089d0f96b6%2FUnique%20Lock%20and%20Lazy%20Initialization%7C81a4eb21-63dc-49b1-9ee2-a34bb45f025b%2F%29&wdorigin=703)

In addition to what’s offered by a `std::lock_guard`, a `std::unique_lock` enables you to

* create it without an associated mutex.
* create it without locking the associated mutex.
* explicitly and repeatedly set or release the lock of the associated mutex.
* move the mutex.
* try to lock the mutex.
* delay the lock on the associated mutex.


---
### Note:

* `std::unique_lock` also support unlock on destroy.
---

### Instant lock

```cpp
{ 

std::unique_lock<mutex> locker(_mu); 
_f << "From" << id << " : " << value << endl;  // std::ostream _f (logFile)

locker.unlock(); 

// do other things, that doesn't require mutex to be locked 
} 
```

### Delayed lock, with unlimited number of times.
```cpp
{ 
std::unique_lock<mutex> locker(_mu, std::defer_lock); 

// do something that doesn't require lock 

locker.lock(); 
_f << "From" << id << " : " << value << endl; 
locker.unlock(); 

// do other things, that doesn't require mutex to be locked 

locker.lock(); 
} 
```

### Moving, `std::lock_guard` can't be moved.
```cpp
std::unique_lock<std::mutex> locker2 = std::move(locker);
// std::lock_guard can't be moved.
```

---
### Note:
* `std::unique_lock` comes with a cost; as compared to `std::lock_guard`
---

### `std::unique_lock` methods

|Method	                                                                    |Description |
|---------------------------------------------------------------------------|------------|
|`lk.lock()`                                                                | Locks the associated mutex. |
|`std::lock(lk1, lk2, ... )`                                                | Locks atomically the arbitrary number of associated mutexes. |
|`lk.try_lock()` and `lk.try_lock_for(relTime)` and `lk.try_lock_until(absTime)`	| Tries to lock the associated mutex. |
|`lk.release()`	                                                            | Release the mutex. The mutex remains locked. |
|`lk.swap(lk2)` and `std::swap(lk, lk2)`	                                    | Swaps the locks. |
|`lk.mutex()`	                                                                | Returns a pointer to the associated mutex. |
|`lk.owns_lock()`	                                                            | Checks if the lock has a mutex. |

---
**Note:**
* `std::lock` can lock atomically the arbitrary number of associated mutexes.
---

### Solution to deadlock using `std::unique_lock`

:heavy_check_mark: *Solution using order of locking*

```cpp
void deadLock(CriticalData& a, CriticalData& b) {
    a.mut.lock();
    b.mut.lock();

    // some processing

    a.mut.unlock();
    b.mut.unlock();
}
```

:heavy_check_mark: *Solution using `std::unique_lock`*

- If you call the constructor of `std::unique_lock` with `std::defer_lock`, the underlying mutex will not be locked automatically.  
- At this point (lines 16 and 21), the `std::unique_lock` is just the owner of the mutex.   
- Thanks to the variadic template `std::lock`, the lock operation is performed in an atomic step (line 25). 
- A variadic template is a template which can accept an arbitrary number of arguments.  
- `std::lock` tries to get all locks in one atomic step, so it either gets all of them or none of them and retries until it succeeds.

```cpp
14. void deadLock(CriticalData& a, CriticalData& b){
15. 
16.   unique_lock<mutex> guard1(a.mut, defer_lock);
17.   cout << "Thread: " << this_thread::get_id() << " first mutex" << endl;
18. 
19.   this_thread::sleep_for(chrono::milliseconds(1));
20. 
21.   unique_lock<mutex> guard2(b.mut, defer_lock);
22.   cout << "  Thread: " << this_thread::get_id() << " second mutex" <<  endl;
23. 
24.   cout << "    Thread: " << this_thread::get_id() << " get both mutex" << endl;
25.   lock(guard1,guard2);
26.   // do something with a and b
27. }
```

In this example, std::unique_lock manages the lifetime of the resources and std::lock locks the associated mutex;   
Otherway round In the first step the mutexes are locked, in the second std::unique_lock manages the lifetime of resources.



## 3. `std::scoped_lock` with c++17

```cpp
// deadlockResolvedScopedLock.cpp
void deadLock(CriticalData& a, CriticalData& b){
  
    std::cout << "Thread: " << this_thread::get_id() << " first mutex" << endl;
    std::this_thread::sleep_for(chrono::milliseconds(1));
    
    std::cout << "  Thread: " << this_thread::get_id() << " second mutex" <<  endl;
    std::cout << "    Thread: " << this_thread::get_id() << " get both mutex" << endl;
   
    std::scoped_lock(a.mut, b.mut);
    // do something with a and b
 }
```

## 4. `std::shared_lock` with c++14

* A `std::shared_lock` has the same interface as a `std::unique_lock` but behaves differently when used with a `std::shared_timed_mutex`
* Many threads can share one `std::shared_timed_mutex` and, therefore, implement a `reader-writer lock`.

### Reader Writer locks

The idea of reader-writer locks is than an arbitrary number of threads executing read operations can access the critical region at the same time, but only one thread is allowed to write.

 * To access the telephone book at the same time, the reading threads use the `std::shared_lock<std::shared_timed_mutex>>`
 * This is in contrast to the writing threads, which need exclusive access to the critical section. The exclusivity is given by the `std::lock_guard<std::shared_timed_mutex>>`

```cpp
// readerWriterLock.cpp

#include <iostream>
#include <map>
#include <shared_mutex>
#include <string>
#include <thread>

std::map<std::string,int> teleBook{{"Dijkstra", 1972}, {"Scott", 1976}, 
                                   {"Ritchie", 1983}};

std::shared_timed_mutex teleBookMutex;

void addToTeleBook(const std::string& na, int tele){
  std::lock_guard<std::shared_timed_mutex> writerLock(teleBookMutex);
  std::cout << "\nSTARTING UPDATE " << na;
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  teleBook[na]= tele;
  std::cout << " ... ENDING UPDATE " << na << std::endl;
}

void printNumber(const std::string& na){
  std::shared_lock<std::shared_timed_mutex> readerLock(teleBookMutex);
  std::cout << na << ": " << teleBook[na];
}

int main(){

  std::cout << std::endl;

  std::thread reader1([]{ printNumber("Scott"); });
  std::thread reader2([]{ printNumber("Ritchie"); });
  std::thread w1([]{ addToTeleBook("Scott",1968); });
  std::thread reader3([]{ printNumber("Dijkstra"); });
  std::thread reader4([]{ printNumber("Scott"); });
  std::thread w2([]{ addToTeleBook("Bjarne",1965); });
  std::thread reader5([]{ printNumber("Scott"); });
  std::thread reader6([]{ printNumber("Ritchie"); });
  std::thread reader7([]{ printNumber("Scott"); });
  std::thread reader8([]{ printNumber("Bjarne"); });

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
  for (auto teleIt: teleBook){
    std::cout << teleIt.first << ": " << teleIt.second << std::endl;
  }

  std::cout << std::endl;
}
```


Fix : 
```cpp
void printNumber(const std::string& na){
  std::shared_lock<std::shared_timed_mutex> readerLock(teleBookMutex);
  auto searchEntry = teleBook.find(na);
  if(searchEntry != teleBook.end()){
    std::cout << searchEntry->first << ": " << searchEntry->second << std::endl;
  }
  else {
    std::cout << na << " not found!" << std::endl;
  }
}
```


### Exclusive vs Shared lock

[Reference](https://stackoverflow.com/questions/11837428/whats-the-difference-between-an-exclusive-lock-and-a-shared-lock)

It's pretty straightforward. Read locks are also known as shared locks because more than one process can read at the same time. The point of a read lock is to prevent the acquisition of a write lock by another process. By contrast, a write lock inhibits all other operations while a write operation completes which is why it is described as exclusive. 

*So a read lock says "you can read now but if you want to write you'll have to wait" whereas a write lock says "you'll have to wait".*

```cpp
std::lock_guard<std::shared_timed_mutex> // Exclusive lock 
std::unique_lock<std::shared_timed_mutex> // Shared lock
```

---
**Note:**
* c++ 17 : std::shared_mutex <==> std::shared_timed_mutex.
---

 
