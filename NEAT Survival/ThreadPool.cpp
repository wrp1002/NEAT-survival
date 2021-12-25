#include "ThreadPool.h"


void ThreadPool::ThreadFunc() {
	while (true) {
		unique_lock<mutex> lock(m);

		cv.wait(lock, [this]() {
			return !jobQueue.empty() || terminate_pool;
		});

		function<void()> Job;
		Job = jobQueue.front();
		jobQueue.pop();

		Job();
	}
}

ThreadPool::ThreadPool() {
	maxThreads = thread::hardware_concurrency();
    terminate_pool = false;


	//for (int i = 0; i < maxThreads; i++) threads.push_back(thread(&ThreadPool::ThreadFunc, this));

	cout << "Initialized thread pool with " << maxThreads << " threads" << endl;
}


void ThreadPool::AddJob(function<void()> newJob) {
    unique_lock<mutex> lock(m);
    jobQueue.push(newJob);
    cv.notify_one();
}


void ThreadPool::AddJobs(vector<function<void()>> &newJobs) {
	unique_lock<mutex> lock(m);

	for (auto newJob : newJobs)
		jobQueue.push(newJob);

	cv.notify_one();
}

bool ThreadPool::HasJobs() {
	return !jobQueue.empty();
}

void ThreadPool::Stop() {
	terminate_pool = true;
}
