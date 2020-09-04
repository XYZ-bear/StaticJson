#pragma once
#include "doctest.h"
#include "static_def.h"
#include "dynamic.h"

TEST_CASE("dynamic add test") {
	dynamic_json dj;

	dj["a"] = 123;
	CHECK(dj.get_int("a") == 123);

	dj["b"] = "abc";
	CHECK(dj.get_string("b") == "abc");

	dj["b"] = "ac";
	CHECK(dj.get_string("b") == "ac");

	dj["b"] = "acddd";
	CHECK(dj.get_string("b") == "acddd");

	dj["c"]["d"] = 4;
	CHECK(dj["c"].get_int("b") == 4);

	dj["c"]["d"] = 14;
	CHECK(dj["c"].get_int("b") == 14);

	//dj["d"] = "abc";
	//CHECK(string(dj.get_string("d")) == "abc");

	//dj["d"].erase();
	//CHECK(string(dj.get_string("d")) == "");
}

TEST_CASE("dynamic test") {
	//dynamic_json dj;
	//dj.unserialize(R({"a":123}));

	//CHECK(dj.get_int("a") == 123);
	//CHECK(dj.get_int("b") == 0);

	//dj.unserialize(R({ "b":"abc" }));
	//CHECK(string(dj.get_string("b")) == "abc");
	//dj["b"] = "jjj";
	//CHECK(string(dj.get_string("b")) == "jjj");


	//dj.unserialize(R({ "b":[1,2,3] }));
	//auto &v = dj["b"];
	//CHECK(v.get_size() == 3);
	//CHECK(v.get_int(0) == 1);
	//CHECK(v.get_int(1) == 2);
	//CHECK(v.get_int(2) == 3);
	//CHECK(v.get_int(3) == 0);
	//CHECK(v.get_int(-1) == 0);

	//dj.unserialize(R({ "b" : {"a":[1,2,3]},"c" : {"d":123} }));
	//CHECK(dj["b"]["a"].get_size() == 3);
	//CHECK(dj["b"]["a"].get_int(2) == 3);
	//CHECK(dj["c"].get_int("d") == 123);

	//dj["c"]["d"] = 456;
	//CHECK(dj["c"].get_int("d") == 456);
}