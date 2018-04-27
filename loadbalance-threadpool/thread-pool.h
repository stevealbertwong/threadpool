/**

 */

#ifndef _thread_pool_
#define _thread_pool_

#include <cstddef>     // for size_t
#include <functional>  // for the function template used in the schedule signature
#include <queue>
#include <thread>
#include <vector>
#include "semaphore.h" 
#include <algorithm>
#include <iomanip>
#include <atomic> // atomic<int>
#include <iostream>

class ThreadPool {
 public:
 	ThreadPool(size_t numThreads);
 	void enqueue(const std::function<void(void)>& thunk);
 	void wait();
	// ~ThreadPool();
 	
 private:
 	// load_balance communicate with workers
 	struct worker_t{		
		unique_ptr<semaphore> ready_to_exec;
		function<void(void)> thunk;
	}typedef worker_t;

	std::vector<worker_t> workers; // single thread access
	
	mutex workers_lock;
	mutex tasks_lock;
	mutex free_threads_lock;
	
	size_t num_active_threads; // single thread access
	atomic<int> tasks_done;

 	unique_ptr<semaphore> max_allowed_sema;
 	unique_ptr<semaphore> tasks_sema;
 	unique_ptr<semaphore> wait_sema;
 	 	  	 	 
 	// for destructor to call join()
 	std::vector<std::thread> worker_threads;
 	// DDOS if queue full, drop request/function
 	// queue_ts + vector_ts => atomic/thread safe
 	queue<function<void(void)> > tasks;
 	queue<int> free_threads;
 	
 	void load_balance();
 	void worker(size_t id);

  // prohibit copying
  // ThreadPool(const ThreadPool& original) = delete;
  // ThreadPool& operator=(const ThreadPool& rhs) = delete;
};

#endif
