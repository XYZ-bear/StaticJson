#pragma once

#include <map>
#include <functional>
#include <time.h>
#include <string>
#include <iostream>
using namespace std;

class perf_test {
public:
	string name;
	clock_t start;
	size_t times;
	size_t count;
	perf_test(const char* n,size_t t) {
		name = n;
		start = clock();
		times = t;
		count = 0;
	}
	bool check_end() {
		if (count++ < times)
			return true;
		else {
			cout << name << ":" << clock() - start << "ms" << endl;
			return false;
		}
	}
};

#define PERF(name,times)					\
perf_test perf_test_##name(#name,times);	\
while(perf_test_##name.check_end())
