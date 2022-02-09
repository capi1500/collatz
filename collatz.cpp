#include "collatz.hpp"

uint64_t calcCollatzShared(InfInt n, std::shared_ptr<SharedResults> sharedResult){
	assert(n > 0);
	if(n == 1)
		return 0;
	
	if(sharedResult->contains(n))
		return sharedResult->get(n);
	
	InfInt mem = n;
	if(n % 2 == 1){
		n *= 3;
		n += 1;
	}
	else
		n /= 2;
	
	uint64_t res = calcCollatzShared(n, sharedResult) + 1;
	sharedResult->memoize(mem, res);
	
	return res;
}

