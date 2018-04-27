/**
Author: Steven Wong
*/
#include "thread-pool.h"

using namespace std;

ThreadPool::ThreadPool(size_t numThreads) {	
	workers = vector<worker_t>(numThreads);

	max_allowed_sema.reset(new semaphore(numThreads)); 	
	tasks_sema.reset(new semaphore(0));
	wait_sema.reset(new semaphore(0));

	num_active_threads = 0;
	tasks_done = 0;


	for (size_t workerID = 0; workerID < numThreads; workerID++) {
		workers[workerID].ready_to_exec.reset(new semaphore(0)); 		
		workers[workerID].thunk = NULL;
	}	

	thread dt([this]() -> void { 
		this->load_balance();
	});
	dt.detach();	
}

// surgery code to join threads
void ThreadPool::wait(){
	wait_sema->wait();	
}

void ThreadPool::enqueue(const std::function<void(void)>& thunk) {	
	tasks_done++;
	tasks_lock.lock();
	tasks.push(thunk);
	tasks_lock.unlock();
		 	
	tasks_sema->signal();
}

void ThreadPool::load_balance() {
	while (true) {

		// wait for function attached
		tasks_sema->wait();		
		// max threads allowed(loop to get threads) 
		max_allowed_sema->wait();		

		// if no free thread, spawn new worker thread
		if(free_threads.empty()){			
			
			tasks_lock.lock(); // protect tasks, no enqueue when send to thread
			workers[num_active_threads].thunk = tasks.front();
			tasks.pop();			
			tasks_lock.unlock();

			std::thread wt([this](size_t num_active_threads) -> void {
				this->worker(num_active_threads);
			}, num_active_threads); // std:bind ??
			wt.detach();
			
			workers[num_active_threads].ready_to_exec->signal();
			num_active_threads++; 

		// if yes existing thread
		}else{
			free_threads_lock.lock();
			int id = free_threads.front();
			free_threads.pop();
			free_threads_lock.unlock();

			tasks_lock.lock();			
			workers[id].thunk = tasks.front();	
			tasks.pop();		
			tasks_lock.unlock();						
			workers[id].ready_to_exec->signal();
		}		
	}
}


/* 
THREAD WILL NOT DIE, IT WILL JUST WAIT TO BE REUSED AFTER ONE LOOP
MULTI-THREADED FUNCTION

worker == web server logic(thunk())
its predicator == load_balance's signal
*/
void ThreadPool::worker(size_t id) {
	while (true) {			
		workers[id].ready_to_exec->wait();
						
		/* NOT LOCKED!!!!! OTHERWISE NOT MULTI-THREADING */		
		workers[id].thunk();
		workers[id].thunk = NULL;
		
		free_threads_lock.lock();
		free_threads.push(id);
		free_threads_lock.unlock();

		max_allowed_sema->signal();	
		
		tasks_done--;
		if(tasks_done == 0){
			wait_sema->signal();
		}
	}
}


