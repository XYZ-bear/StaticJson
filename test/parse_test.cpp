#pragma once
#include "doctest.h"
#include "static_json.h"
#include <tchar.h>

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
	CHECK(t.f.size() == 2);
	CHECK(t.g == 0);
	//CHECK(t.h.a == 0);
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

	t.unserialize(R({ "b" : 5}));
	CHECK(t.b == 1);

	t.unserialize(R({ "b" : fal ,"c":"yes"}));
	CHECK(t.c == "yes");
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


	t.unserialize(R({ "vec":[] }));
	CHECK(t.vec.size() == 0);

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

	t.unserialize(R({ "str":" 123","n" : 123,"jj" : {"fs":123}, "t" : {"str2":"456","n2" : 456} }));
	CHECK(t.str == " 123");
	CHECK(t.n == 123);
	CHECK(t.t.str2 == "456");
	CHECK(t.t.n2 == 456);
}

Json(Test2) {
public:
	string N(str);
	int N(i);
};

TEST_CASE("char test") {
	Test2 t;

	t.unserialize(R({ "str":"","i":1234 }));
	CHECK(t.str == "");
	CHECK(t.i == 1234);

	t.unserialize(R({ "str":" 123" }));
	CHECK(t.str == " 123");

	t.unserialize(R({ "str":  "123" }));
	CHECK(t.str == "123");

	t.unserialize(R({ "str":"\"http:\\www.google.com\"" }),UNESCAPE);
	CHECK(t.str == "\\\"http:\\\\www.google.com\\\"");

	t.unserialize(R({ "str":"\\" }), UNESCAPE);
	CHECK(t.str == "\\\\");

	t.unserialize(R({ "str":"\\\"" }), UNESCAPE);
	CHECK(t.str == "\\\\\\\"");

	t.unserialize(R({ "str":"\abc" }), UNESCAPE);
	CHECK(t.str == "\\abc");

	t.unserialize(R({ "str":"\u4f60" }),UNESCAPE_UNICODE);
	CHECK(t.str == "\\u4f60");
	
	t.unserialize(R({ "str":"\n" }), UNESCAPE);
	CHECK(t.str == "\\n");

	t.unserialize(R({ "str":"\n" }));
	CHECK(t.str == "\n");
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