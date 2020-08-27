#pragma once
#include "doctest.h"
#include "static_def.h"
#include "perf.h"

void get_file(string path, string &res) {
	ifstream myfile(path);
	string temp;
	while (getline(myfile, temp)) {
		res += temp;
	}
	myfile.close();
}

Json(D) {
public:
	double N(d);
};

TEST_CASE("twitter test") {
	PERF(canada_test, 100000000) {
		D d;
		d.unserialize(R({ "d":1.7976931348623157 }));
	}


	string res;
	get_file(".//data//twitter.json",res);
	PERF(twitter_test, 100) {
		Twitter twitter;
		twitter.unserialize(res.data());
	}
}

TEST_CASE("cananda test") {
	string res;
	get_file(".//data//canada.json", res);
	PERF(canada_test, 1) {
		Canada canada;
		canada.unserialize(res.data());
	}
}
