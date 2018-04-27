#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <mutex>
#include <condition_variable>

using namespace std;
class semaphore{

public:

	semaphore(int count=0) : count(count) {}

	void signal(){
		lock_guard<mutex> lg(m);
		count++;
		if(count == 1) cv.notify_all(); // only notify all to check condition when 0->1
	}

	// conditional lock
	void wait(){
		lock_guard<mutex> lg(m);
		// [this]: "this" need to be captured by on the fly predicate
		// capturing addr of surrounding object allow access of count
		// [&wait]: works with g++, but buggy for other compilers
		// cv.wait(numAllowed >0);
		// lock acquired to check whether predicate met
		cv.wait(m,[this]{return count > 0;} ); // predicate/condition lambda
		count--;

	}

private:
	int count;
	mutex m;
	condition_variable_any cv;
};




#endif