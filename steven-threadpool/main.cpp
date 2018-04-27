/*
g++ -std=c++11 main.cpp -o main

note: function in for loop needs to return since results is vector of future<int>
*/
#include <iostream>
#include <vector>
#include <chrono>
#include <string>

#include "threadpool.h"


class functor
{
    public:
        int operator()() const
        {
            std::cout << "I'm a functor" << std::endl;
            return 10;
        }
};

class functor_arg
{
    public:
        int operator()(int i) const
        {
            std::cout << "I'm a functor" << std::endl;
            return i*i;
        }
};

int normal_func(int id, const std::string & s) {
    std::cout << "normal function " << id << ' ' << s << '\n';
    return 0;
}

void no_return(){
    std::cout << "hi" << std::endl;
}

auto func = std::function<int()>(
    [](){
    std::cout << "HI, I'm a lambda expression almost same" << std::endl;
    return 10;
    }   
);

int main()
{        
    ThreadPool pool(4);   
    functor functor_instance; 
    functor_arg functor_instance2; 
    
    std::vector< std::future<int> > results;
    
    for(int i = 0; i < 10; ++i) {
        results.emplace_back(            
            pool.enqueue(func)                
        );        
        results.emplace_back(            
            pool.enqueue(functor_instance)                
        );
        results.emplace_back(            
            pool.enqueue(std::ref(functor_instance))
        );
        results.emplace_back(            
            pool.enqueue(std::move(functor_instance))
        );
        results.emplace_back(            
            pool.enqueue(const_cast<const functor &>(functor_instance))
        );
        results.emplace_back(
            pool.enqueue(functor_instance2, 2)      
        );
        results.emplace_back(
            pool.enqueue(normal_func, i , "hello world")      
        );
        results.emplace_back(
            pool.enqueue([i]{        
                std::cout << "hello from " << i << std::endl;
                return i*i;
            })
        );    
    }

    for(auto && result: results)
        std::cout << result.get() << ' ';
    std::cout << std::endl;

    auto fu = pool.enqueue(no_return);
    fu.get(); 



    return 0;
}
