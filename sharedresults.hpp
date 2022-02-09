#pragma once

#include <shared_mutex>
#include <map>
#include "lib/infint/InfInt.h"

class SharedResults{
	private:
		std::map<std::string, uint64_t> memory;
		mutable std::shared_mutex m;
	public:
		bool contains(const InfInt& val) const{
			std::shared_lock<std::shared_mutex> l(m);
			return memory.count(val.toString()) != 0;
		}
		
		uint64_t get(const InfInt& val) const{
			std::shared_lock<std::shared_mutex> l(m);
			return memory.at(val.toString());
		}
		
		void memoize(const InfInt& val, uint64_t res){
			std::unique_lock<std::shared_mutex> l(m);
			memory[val.toString()] = res;
		}
};