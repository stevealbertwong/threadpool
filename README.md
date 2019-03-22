# threadpool

*architecture*
![alt text](https://raw.githubusercontent.com/stevealbertwong/threadpool/master/web_architecture.png)

## TODO (loadbalance thread)
1. Use wait to join threads, currently surgery code to use condition variable and detached() to wait for all threads to finish.
Concretely, need to stop thread cv predicate and quit while true loop and use std::vector<std::thread> to store spawned threads and destructor to join().

2. safequeue.h + safevector.h 
Right now using lock() and unlock(), very clumsy and unclean. But for the purpose of synchronization logic. 
