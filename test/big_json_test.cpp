#pragma once
#include "doctest.h"
#include "static_def.h"
#include "perf.h"
#include<thread>

void get_file(string path, string &res) {
	res.resize(0);
	ifstream myfile(path);
	string temp;
	while (getline(myfile, temp)) {
		res += temp;
		res += '\n';
	}
	myfile.close();
}

Json(D) {
public:
	double N(d);
};

TEST_CASE("twitter test") {
	PERF(canada_test,1) {
		D d;
		d.unserialize(R({ "d":1.7976931348623157 }));
	}

	string res;
	get_file(".//data//twitter.json",res);
	PERF(twitter_test, 1) {
		Twitter twitter;
		twitter.unserialize(res.data());
	}
}

TEST_CASE("multi thread test") {
	string res;
	get_file(".//data//twitter.json", res);

	thread t1([&res]()->void {
		PERF(t1, 10) {
			Twitter twitter;
			CHECK(twitter.unserialize(res.data()));
		}
	});

	thread t2([&res]()->void {
		PERF(t2, 10) {
			Twitter twitter;
			CHECK(twitter.unserialize(res.data()));
		}
	});
	t1.join();
	t2.join();
}

TEST_CASE("cananda test") {
	string res;
	get_file(".//data//canada.json", res);
	PERF(canada_test, 0) {
		Canada canada;
		canada.unserialize(res.data());
	}
}