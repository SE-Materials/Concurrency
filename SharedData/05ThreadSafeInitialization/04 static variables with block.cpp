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