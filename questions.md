#### How does construct of std::thread takes reference like std::ref ?

#### class reference member

#### Thread methods
```cpp
std::thread::hardware_concurrency()
std::this_thread::sleep_until(absTime)
std::this_thread::sleep_for(relTime)
std::this_thread::yield()
t.swap(t2) and std::swap(t1, t2) // Threads can not be copied but can be moved. swap does it when it's possible
```


|Method              	    | mutex  |recursive_mutex |  timed_mutex  | recursive_timed_mutex	| shared_timed_mutex  |
|---------------------------|--------|----------------|---------------|-------------------------|---------------------|
|m.lock	                    | yes	 |   yes	      |       yes	  |         yes	            |           yes       |
|m.unlock	                | yes	 |   yes	      |       yes	  |         yes	            |           yes       |
|m.try_lock	                | yes	 |   yes	      |       yes	  |         yes	            |           yes       |
|m.try_lock_for	            | no	 |   no	          |       yes	  |         yes             |         	yes       |
|m.try_lock_until	        | no	 |   no	          |       yes	  |         yes	            |           yes       |
|m.try_lock_shared	        | yes	 |   no	          |       no      |        	no              |         	yes       |
|m.try_lock_shared_for	    | no	 |   no           |	      no      |     	no              |         	yes       |
|m.try_lock_shared_until 	| no  	 |   no           |  	  no      |     	no              |         	yes       |