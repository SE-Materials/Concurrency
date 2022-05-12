# Exceptions and Unknown Behaviour

In the following snippet.. 
* Never ever call an unknown function while holding a lock. 
    * `getVar()` can throw an exception, and mutex will never be released.
    * `getVar()` tries to acquire the lock again on `m`. Since `m` is not a recursive mutex; deadlock.
    
```cpp
std::mutex m;

m.lock();
sharedVariable = getVar(); 
m.unlock();
```