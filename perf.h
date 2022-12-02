#pragma once

#include <map>
#include <functional>
#include <time.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <mutex>
using namespace std;

// class perf_test {
// public:
// 	string name;
// 	clock_t start;
// 	size_t times;
// 	size_t count;
// 	size_t cost;
// 	perf_test(const char* n,size_t t) {
// 		name = n;
// 		start = clock();
// 		times = t;
// 		count = 0;
// 		cout.setf(ios::left);
// 	}
// 	bool check_end() {
// 		if (count++ < times)
// 			return true;
// 		else {
// 			cost = clock() - start;
// 			static mutex mtx;
// 			mtx.lock();
// 			cout <<"[perftest]"  << setw(40) << name << ":" << cost << "ms";
// 			cout << "[" << times <<"]"<< endl;
// 			mtx.unlock();
// 			return false;
// 		}
// 	}
// };

// #define PERF(name,times)					\
// perf_test perf_test_##name(#name,times);	\
// while(perf_test_##name.check_end())




class perf_test {
public:
	string name;
	clock_t start;
	size_t times;
	size_t cost;
	perf_test(const char* n,size_t t) {
		name = n;
		start = clock();
		times = t;
		cout.setf(ios::left);
	}
	bool check_end(size_t *count) {
		if (*count < times)
			return true;
		else {
			cost = clock() - start;
			static mutex mtx;
			mtx.lock();
			cout <<"[perftest]"  << setw(40) << name << ":" << cost << "ms";
			cout << "[" << times <<"]"<< endl;
			mtx.unlock();
			return false;
		}
	}
};


#define PERF(name,times)					\
perf_test perf_test_##name(#name,times);	\
for(size_t _idx = 0; perf_test_##name.check_end(&_idx); _idx++)
