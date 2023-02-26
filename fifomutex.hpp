#include <mutex>
#include <iostream>
#include <condition_variable>

class FIFOMutex { 
	private:
	std::condition_variable cv;
	std::mutex m;
    	unsigned long queue_head, queue_tail; 

	public:
	FIFOMutex() : cv{}, m{}, queue_head(0), queue_tail(0) {};

	void lock() {
	    unsigned long queue_me;
	    std::unique_lock<std::mutex> lk{m};
	    queue_me = queue_tail++; 
	    while (queue_me != queue_head) { cv.wait(lk); };
	    lk.unlock();
	}


	void unlock() {
	    std::unique_lock<std::mutex> lk{m};
	    queue_head++;
	    cv.notify_all();
	    lk.unlock();
	}
}; 

