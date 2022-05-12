# Thread local data

* Thread-local data, also known as thread-local storage, will be created for each thread separately. 
* It behaves like static data because it’s bound for the lifetime of the thread and it will be created at its first usage. 
* Also, thread-local data belongs exclusively to the thread.

---
### From a Single-Threaded to Multithreaded Program

Thread-local data helps to port a single-threaded program to a multithreaded environment. If the global variables are thread-local, there is the guarantee that each thread will get its own copy of the data. Due to this fact, there is no shared mutable state which may cause a data race resulting in undefined behavior.


In contrast to thread-local data, condition variables are not easy to use.
---