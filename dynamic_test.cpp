#pragma once
#include "doctest.h"
#include "static_def.h"
#include "dynamic.h"
#include "perf.h"

TEST_CASE("dynamic add test") {
	dynamic_json dj;

	dj["a"] = 123;
	string kk = dj["a"];

	CHECK((dj["a"] == 123));
	CHECK(dj["a"].is_number() == true);

	dj["a"] = "hello";
	CHECK((dj["a"] == "hello"));
	CHECK(dj["a"].is_string() == true);
	CHECK(dj["a"].size() == 5);

	dj["b"] = "abc";
	CHECK((dj["b"] == "abc"));

	dj["b"] = "ac";
	CHECK((dj["b"] == "ac"));

	dj["b"] = "acddd";
	CHECK((dj["b"] == "acddd"));

	dj["c"]["d"] = 4;
	CHECK((dj["c"]["d"] == 4));
	CHECK(dj["c"].is_object() == true);

	dj["c"]["d"] = 14;
	CHECK((dj["c"]["d"] == 14));

	dj["c"]["a"] = 12;
	CHECK((dj["c"]["a"] == 12));

	dj["c"]["a"].erase();
	CHECK(!(dj["c"]["a"] == 0));

	dj["c"].erase();
	CHECK(dj.find("c")== false);
	
}


TEST_CASE("araay test") {
	dynamic_json dj;

	//dj[0] = 123;
	//dj[1] = 456;
	//CHECK(dj.get_int(0) == 123);
	//CHECK(dj.get_int(1) == 456);
	//CHECK(dj.is_array() == true);

	//dj is array, can't add key-val
	//dj["a"][0] = 456;
	//CHECK(!(dj["a"][0] == 0));

	dynamic_json dj2;
	dj2["c"]["d"] = 14;
	dj2["c"]["e"] = "hello";
	dj2["d"]["e"] = "e";
	dj2["d"]["b"] = 123;
	dj2["e"][0] = 123;
	dj2["e"][1] = 456;
	dj2["f"][0] = "a";
	dj2["f"][1] = "b";
	dj2["e"][2] = 789;
	dj2["k"][0]["a"] = 789;
	//cout << dj2["c"]["d"].is_number();
	dj2.dump();
}