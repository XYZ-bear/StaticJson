#pragma once
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "static_json.h"

#include <map>
#include <functional>
#include <time.h>
#include <string>
#include <iostream>
using namespace std;

void perf_test(string name, size_t t, function<void()> f) {
	clock_t startTime, endTime;
	startTime = clock();
	for (size_t i = 0; i < t; i++)
		f();
	endTime = clock();
	cout << name << ":" << endTime - startTime << "ms" << endl;
}

Json(Basic) {
public:
	std::string N(cid);// : "CN101010100",
	std::string	N(location);// " : "北京",
	std::string	N(parent_city);// : "北京",
	std::string	N(admin_area);// : "北京",
	std::string	N(cnty);// : "中国",
	std::string	N(lat);// : "39.90498734",
	std::string	N(lon);// : "116.40528870",
	std::string	N(tz);// : "8.0"
	int N(ca);
};


#define R(...) #__VA_ARGS__

TEST_CASE("perf test") {
	perf_test("table build 1", 1000000, []()->void {
		Basic f;
		f.unserialize(R(
			{
				"cid": "CN101010100",
				"location" : "北京",
				"parent_city" : "北京",
				"admin_area" : "北京",
				"cnty" : "中国",
				"lat" : "39.90498734",
				"lon" : "116.40528870",
				"tz" : "8.0"
			}));
	});
	getchar();
}

Json(Test2) {
public:
	string N(str);
};

TEST_CASE("char test") {
	Test2 t;

	t.unserialize(R({ "str":" 123" }));
	CHECK(t.str == " 123");

	t.unserialize(R({ "str":  "123" }));
	CHECK(t.str == "123");

	t.unserialize(R({ "str":"\"http:\\www.google.com\"" }));
	CHECK(t.str == "\\\"http:\\\\www.google.com\\\"");
}

Json(Num) {
public:
	int64_t N(n);
};

TEST_CASE("Scientific counting test") {
	Num n;
	n.unserialize(R({ "n":1.04577616461583E+15 }));
	CHECK(n.n == 1045776164615830);

	n.n = 0;
	n.unserialize(R({ "n":1.04577616461583e+15 }));
	CHECK(n.n == 1045776164615830);

	n.n = 0;
	n.unserialize(R({ "n":-1.04577616461583e+15 }));
	CHECK(n.n == -1045776164615830);

	n.n = 0;
	n.unserialize(R({ "n":1.04e+2 }));
	CHECK(n.n == 104);
}