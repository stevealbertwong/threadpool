# threadpool

## TODO (loadbalance thread)
Use wait to join threads, currently surgery code to use condition variable and detached() to wait for all threads to finish.
Concretely, need to stop thread cv predicate and quit while true loop and use std::vector<std::thread> to store spawned threads and destructor to join().
