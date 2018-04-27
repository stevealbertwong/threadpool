/*
g++ -std=c++11 example.cc thread-pool.cc -o example

note: function in for loop needs to return since results is vector of future<int>
*/


#include "thread-pool.h"
using std::cout;
using std::endl;

class functor
{
    public:
        void operator()() const
        {
            std::cout << "I'm a functor" << std::endl;            
        }
};

void no_return(){
    std::cout << "hellllo world" << std::endl;
}

auto func = std::function<void()>(
    [](){
    std::cout << "HI, I'm a lambda expression" << std::endl;    
    }   
);

int main()
{        
    ThreadPool pool(4);   
    functor functor_instance;
    for (int i = 0; i < 10; ++i)
    {
        // pool.enqueue(functor_instance);
        // pool.enqueue(no_return);
        pool.enqueue(func);
    }            
    pool.wait();
    // usleep(5000000); // 5 secs    
    

    return 0;
}
