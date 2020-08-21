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

class Basic1 {
public:
	std::string (cid);// : "CN101010100",
	std::string	(location);// " : "北京",
	std::string	(parent_city);// : "北京",
	std::string	(admin_area);// : "北京",
	std::string	(cnty);// : "中国",
	std::string	(lat);// : "39.90498734",
	std::string	(lon);// : "116.40528870",
	std::string	(tz);// : "8.0"
	int ca;

	const char* begin;
	const char* end;
	const char* next;
	char ch;
};

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


TEST_CASE("constructor test") {
	//CHECK(json_base_t<Basic>::inited_ == false);
	//Basic f;
	//CHECK(json_base_t<Basic>::inited_ == true);
}

TEST_CASE("perf test") {
	//perf_test("table build 1", 1000000, []()->void {
	//	Basic f;
	//});
	//perf_test("table build 1", 1000000, []()->void {
	//	Basic1 f;
	//});
	//getchar();
}
//
Json(Complex)
{
public:
	Json(profile_t) {
	public:
	string N(lastName);
	vector<vector<int>> N(firstName);
	int64_t N(age);
	string N(gender);
	Json(contact_t) {
		string N(type);
		string N(number);
	};
	vector<contact_t> N(contact);
	Json(address_t) {
		string N(postal_code);
		string N(state);
		string N(street);
		string N(city);
	};
	address_t N(address);
	bool N(marital_status);
	};
	profile_t N(profile);
};

class Complex2
{
public:
	class profile_t {
public:
	string (lastName);
	vector<vector<int>> (firstName);
	int64_t (age);
	string (gender);
	class contact_t {
		contact_t() {

		}
		string (type);
		string (number);
	};
	vector<contact_t> (contact);
	class address_t {
		string (postal_code);
		string (state);
		string (street);
		string (city);
	};
	address_t (address);
	bool (marital_status);
	};
	profile_t (profile);
};

TEST_CASE("complex test") {
	//perf_test("table build 1", 1000000, []()->void {
	//	Complex t;

	//	t.unserialize(R(
	//		{
	//			"profile":
	//	{
	//		"firstName": [[1,2],[4,5]],
	//			"lastName" : "&#x4F60;&#x597D;",
	//			"age" : 1.04577616461583E+15,
	//			"gender" : "Male",
	//			"address" :
	//		{
	//			"street": "20th 2nd Street",
	//				"city" : "New York",
	//				"state" : "NY",
	//				"postal_code" : "10003"
	//		},
	//			"contact": [
	//			{
	//				"type": "Home",
	//					"number" : "(735) 754-0100"
	//			},
	//				{
	//					"type": "Office",
	//					"number" : "(725) 854-0750"
	//				}],
	//				"marital_status": true
	//	}
	//		}));
	//});
	//perf_test("table build 1", 1000000, []()->void {
	//	Complex2 t;
	//});
	//getchar();
}

Json(Test6)
{
public:
	bool N(b);
	string N(c);
	int64_t N(d);
	vector<int> N(e);
	vector<int> N(f);
	int N(g);

	Json(Test)
	{
	public:
		int N(a);
	};
	Test N(h);
	int N(i);
};

#include <string>
#include <iostream>

TEST_CASE("compatibility test") {
	Test6 t;

	//if a value not match the static type,the next value should be paresd.
	t.unserialize(R({ "f" : {},"g" : 123));
	CHECK(t.f.size() == 0);
	CHECK(t.g == 123);

	t.unserialize(R({ "b":true,"d" : "123","c" : 123,"e" : "123","f" : [123,"456"],"g" : {},"h":"123","i":123 }));
	CHECK(t.b == true);
	CHECK(t.d == 0);
	CHECK(t.c == "");
	CHECK(t.e.size() == 0);
	CHECK(t.f.size() == 1);
	CHECK(t.g == 0);
	CHECK(t.h.a == 0);
	CHECK(t.i == 123);

	t.unserialize(R({ "b":[ ,"d" : 123}));
	CHECK(t.b == false);
	CHECK(t.d == 0);

	t.unserialize(R({ "b":[],"d":123 }));
	CHECK(t.b == false);
	CHECK(t.d == 123);

	t.unserialize(R({ "b":"fff","d" : 123 }));
	CHECK(t.b == false);
	CHECK(t.d == 123);

	t.unserialize(R({ "b":{},"d" : 123 }));
	CHECK(t.b == false);
	CHECK(t.d == 123);

	t.unserialize(R({ "b":tr }));
	CHECK(t.b == false);
}

Json(Test5)
{
public:
	vector<int> N(a);
	bool N(b);
	string N(c);
	int64_t N(d);
};

TEST_CASE("bool,null test") {
	Test5 t;

	t.unserialize(R({"b":true }));
	CHECK(t.b == true);

	t.unserialize(R({ "a":null, "b" : null,"c":null,"d":null }));
	CHECK(t.a.size() == 0);
	CHECK(t.b == false);
	CHECK(t.c == "");
	CHECK(t.d == 0);
}

Json(Test4)
{
public:
	vector<int> N(vec);
	vector<string> N(sv);
	vector<vector<int>> N(vec2);
	Json(Te) {
	public:
		int N(a);
		string N(b);
	};
	vector<Te> N(vec3);
	vector<vector<Te>> N(vec4);
};

TEST_CASE("vector test") {
	Test4 t;

	t.unserialize(R({ "vec":[12,3]}));
	CHECK(t.vec == vector<int>({12,3}));

	t.vec.clear();
	t.unserialize(R({ "vec": [ 12, 3 , 4 ] }));

	CHECK(t.vec == vector<int>({ 12,3,4 }));

	t.unserialize(R({ "sv":["he","llo"] }));
	CHECK(t.sv == vector<string>({ "he","llo" }));

	t.unserialize(R({ "vec2":[[123,356],[789,1011]] }));
	CHECK(t.vec2 == vector<vector<int>>({ {123,356},{789,1011} }));

	t.unserialize(R({ "vec3":[{"a":123,"b":"yes"},{"a":456,"b":"no"}] }));
	CHECK(2 == t.vec3.size());
	CHECK(123 == t.vec3[0].a);
	CHECK("yes" == t.vec3[0].b);
	CHECK(456 == t.vec3[1].a);
	CHECK("no" == t.vec3[1].b);

	t.unserialize(R({ "vec4":[[{"a":123,"b" : "yes"},{ "a":456,"b" : "no" }],[{"a":123,"b" : "yes"},{ "a":456,"b" : "no" }]] }));
	CHECK(2 == t.vec4.size());
	CHECK(2 == t.vec4[0].size());
	CHECK(2 == t.vec4[1].size());
	CHECK(123 == t.vec4[0][0].a);
	CHECK("yes" == t.vec4[0][0].b);
	CHECK(456 == t.vec4[0][1].a);
	CHECK("no" == t.vec4[0][1].b);
	CHECK(123 == t.vec4[1][0].a);
	CHECK("yes" == t.vec4[1][0].b);
	CHECK(456 == t.vec4[1][1].a);
	CHECK("no" == t.vec4[1][1].b);
}


Json(Test3)
{
public:
	string N(str);
	int N(n);
	Json(t_t) {
	public:
		string N(str2);
		int N(n2);
	};
	t_t N(t);
};

TEST_CASE("object test") {
	Test3 t;

	t.unserialize(R({ "str":" 123","n" : 123,"t" : {"str2":"456","n2" : 456} }));
	CHECK(t.str == " 123");
	CHECK(t.n == 123);
	CHECK(t.t.str2 == "456");
	CHECK(t.t.n2 == 456);
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


	n.n = 0;
	n.unserialize(R({ "n":1.0sfs }));
	CHECK(n.n == 1);

	n.n = 0;
	n.unserialize(R({ "n":asf1.0sfs }));
	CHECK(n.n == 0);

	n.n = 0;
	n.unserialize(R({ "n":null }));
	CHECK(n.n == 0);
}