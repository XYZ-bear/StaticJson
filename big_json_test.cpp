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

TEST_CASE("twitter test") {
	string res;
	get_file(".//data//twitter.json",res);
	PERF(twitter_test, 1) {
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
