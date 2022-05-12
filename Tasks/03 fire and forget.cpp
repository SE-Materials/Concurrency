#include<iostream>
#include<future>

int main(){
  
    auto fut = std::async([]{ return 2011; });
    std::cout << fut.get() << std::endl;         
    
    std::async(std::launch::async, 
            []{ std::cout << "fire and forget" << std::endl; });
}