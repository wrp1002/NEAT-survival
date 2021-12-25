#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>
#include <iostream>

using namespace std;

class ThreadPool {
private:
	int maxThreads;
	vector<thread> threads;
	queue<function<void()>> jobQueue;
	condition_variable cv;
	mutex m;
	bool terminate_pool;


public:
	ThreadPool();
	void ThreadFunc();
	void AddJob(function<void()> newJob);
	void AddJobs(vector<function<void()>> &newJobs);

	bool HasJobs();

	void Stop();

};

