/*
Author: Steven Wong

worker thread (operator())
1. cv.wait() + predicate for each thread
2. move + pop
3. task()


dispatcher thread (enqueue())
1. packaged_task
2. get_future()
3. lock_guard + emplace
4. cv.notify()

g++ -std=c++11 -g -O0 main.cpp -o main
*/
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <iostream>

#define LOG(x) std::cout << x << std::endl;

class ThreadPool
{
public:
    ThreadPool(int threads) : stop(false), numThreads(threads), workers(std::vector<std::thread>(threads)) {this->init();};    
    
    template<typename F, typename...Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;


    template<typename F>
    auto enqueue(F && f) 
        ->std::future<decltype(f(0))>;
    
    ~ThreadPool();

private:
    void init();
    bool stop; 
    size_t numThreads;
    std::mutex queue_mutex;
    std::condition_variable cv;
    std::vector< std::thread > workers;
    std::queue< std::function<void()> > tasks;

    // functor => web server logic e.g. memcached, DB access(replicate, partition, specialize)
    class ThreadWorker {
    private:
      int tid;      
      ThreadPool * tpool; // nested class access same parent(reference) class's variable
    public:      
      ThreadWorker(ThreadPool * pool, const int id)
        : tpool(pool), tid(id) {}

      void operator()() {        
        while (true) {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lock(tpool->queue_mutex);
            tpool->cv.wait(lock, [this]{return this->tpool->stop || !this->tpool->tasks.empty();});
            if(tpool->stop && tpool->tasks.empty())
                return;
            task = std::move(tpool->tasks.front());
            tpool->tasks.pop();            
          }
          task();
          }
      }   
    };
};

inline void ThreadPool::init(){
    // naive load balancing
    for (int i = 0; i < workers.size(); ++i) {        
      workers[i] = std::thread(ThreadPool::ThreadWorker(this, i));
    }
}


// with arguments
template<typename F, typename...Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> // TODO return type??
{
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared< std::packaged_task<return_type()> >(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");                
        tasks.emplace([task](){ (*task)(); });
    }
    cv.notify_one();
    return res;
}

// function overload without arguments
template<typename F>
auto ThreadPool::enqueue(F && f) 
    ->std::future<decltype(f(0))> 
{    
    auto task = std::make_shared<std::packaged_task<decltype(f(0))(int)>>(std::forward<F>(f));
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");                
        tasks.emplace([task](){ (*task)(); });
    }
    cv.notify_one();
    return task->get_future();
}

inline ThreadPool::~ThreadPool(){
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    cv.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}
#endif




