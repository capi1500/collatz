#include <future>
#include <condition_variable>

#include "teams.hpp"
#include "contest.hpp"

ContestResult TeamNewThreads::runContestImpl(ContestInput const& contestInput){
	ContestResult r(contestInput.size());
	std::vector<std::thread> threads;
	std::atomic<std::size_t> working = 0;
	std::mutex mutex;
	std::condition_variable cond;
	
	auto job = [this, &r, &contestInput, &cond, &working](uint32_t id){
		r[id] = compute(contestInput[id]);
		working--;
		cond.notify_one();
	};
	
	for(uint32_t i = 0; i < contestInput.size(); i++){
		while(working == getSize()){
			std::unique_lock<std::mutex> lock(mutex);
			cond.wait(lock);
		}
		working++;
		threads.emplace_back(std::move(createThread(job, i)));
	}
	for(auto& t : threads)
		t.join();
	
	return r;
}

ContestResult TeamConstThreads::runContestImpl(ContestInput const& contestInput){
	ContestResult r(contestInput.size());
	std::vector<std::thread> threads;
	
	auto job = [this, &r, &contestInput](uint32_t id){
		uint32_t i = id;
		while(i < r.size()){
			r[i] = compute(contestInput[i]);
			i += getSize();
		}
	};
	
	for(uint32_t i = 0; i < getSize(); i++)
		threads.emplace_back(createThread(job, i));
	
	for(auto& t : threads)
		t.join();
	
	return r;
}

ContestResult TeamPool::runContest(ContestInput const& contestInput){
	std::vector<std::future<uint64_t>> futures;
	futures.reserve(contestInput.size());
	
	for(const auto& val : contestInput){
		if(getSharedResults())
			futures.emplace_back(pool.push(calcCollatzShared, val, getSharedResults()));
		else
			futures.emplace_back(pool.push(calcCollatz, val));
	}
	
	return cxxpool::get(futures.begin(), futures.end(), std::vector<uint64_t>());
}

ContestResult TeamNewProcesses::runContest(ContestInput const& contestInput){
	ContestResult r;
	//TODO
	return r;
}

ContestResult TeamConstProcesses::runContest(ContestInput const& contestInput){
	ContestResult r;
	//TODO
	return r;
}

ContestResult TeamAsync::runContest(ContestInput const& contestInput){
	ContestResult r;
	std::vector<std::future<uint64_t>> futures;
	r.reserve(contestInput.size());
	futures.reserve(contestInput.size());
	
	for(auto& val : contestInput)
		futures.push_back(std::async(&TeamAsync::compute, this, val));
		
	for(auto& f : futures)
		r.push_back(f.get());
	
	return r;
}
