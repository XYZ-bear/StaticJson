#pragma once
#include "doctest.h"
#include "static_def.h"
#include "dynamic.h"
#include "perf.h"

extern void get_file(string path, string &res);

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

TEST_CASE("num v test") {
	dynamic_json dj;
	dj.unserialize(R({ "a":"\u4f60" }));
}

TEST_CASE("num limit test") {
	dynamic_json dj;

	dj["a"] = 18446744073709551615;
	CHECK((dj["a"] == 18446744073709551615));

	dj.unserialize(R({ "a":18446744073709551615 }));
	CHECK((dj["a"] == 18446744073709551615));

	dj["a"] = 9, 223, 372, 036, 854, 775, 808;
	CHECK((dj["a"] == 9, 223, 372, 036, 854, 775, 808));

	dj["a"] = -9, 223, 372, 036, 854, 775, 808;
	CHECK((dj["a"] == -9, 223, 372, 036, 854, 775, 808));

	dj["a"] = 1.79769e+308;
	CHECK((dj["a"] == 1.79769e+308));

	dj["a"] = 2.22507e-308;
	CHECK((dj["a"] == 2.22507e-308));

	dj["a"] = 23456789012E66;
	CHECK((dj["a"] == 23456789012E66));
	
	dj.unserialize(R({ "a":23456789012e66 }));
	CHECK((dj["a"] == 23456789012E66));

	dj.unserialize(R({ "a":1.111 }));
	CHECK((dj["a"] == 1.111));
}

TEST_CASE("araay test") {
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

TEST_CASE("parse test") {
	dynamic_json dj;

	dj.unserialize(R({"a":2334}));
	CHECK((dj["a"] == 2334));

	dj.unserialize(R({ "a":"hello" }));
	CHECK((dj["a"] == "hello"));

	dj.unserialize(R({ "a": [123,456] }));
	dj.dump();
	CHECK((dj["a"][0] == 123));
	CHECK((dj["a"][1] == 456));
	CHECK(dj["a"].size() == 2);

	dj.unserialize(R({ "a": [[1,2],[3,4,5]] }));
	CHECK(dj["a"].size() == 2);
	CHECK(dj["a"][0].size() == 2);
	CHECK(dj["a"][1].size() == 3);
	CHECK((dj["a"][0][1] == 2));
	CHECK((dj["a"][1][2] == 5));

	dj.unserialize(R({ "a": [{"b":8},234,"abc",[123]] }));
	CHECK((dj["a"].size() == 4));
	CHECK((dj["a"][0]["b"] == 8));
	CHECK((dj["a"][1] == 234));
	CHECK((dj["a"][2] == "abc"));
	CHECK((dj["a"][3][0] == 123));

	dj.unserialize(R({ "a": [],"b" : {} }));
	cout << dj["a"].size();
	CHECK((dj["a"].size() == 0));
	CHECK((dj["b"].size() == 0));

	CHECK(dj.unserialize(R({])) == 0);
	CHECK(dj.unserialize(R({"":12})));

	CHECK(dj.unserialize(R({ "a": [{"b":8},234,"abc",[123]] }}}})) == 0);
	CHECK(dj.unserialize(R(,"a": [{"b":8},234,"abc",[123]] }}}})) == 0);

	CHECK(dj.unserialize(R({ "a": [[[1,2],[3,4,5]],[[1,2],[3,4,5]]] })));

	for (int i = 1; i < 34; i++) {
		string path = ".//data//jsonchecker//fail";
		if (i < 10)
			path += '0';
		path += to_string(i);
		path += ".json";
		string js;
		get_file(path, js);
		
		if (js.size() > 0) {
			cout << path << ":";
			dynamic_json dj;
			cout << dj.unserialize(js.data()) << endl;
		}
	}

	for (int i = 1; i < 4; i++) {
		string path = ".//data//jsonchecker//pass";
		if (i < 10)
			path += '0';
		path += to_string(i);
		path += ".json";
		string js;
		get_file(path, js);

		if (js.size() > 0) {
			cout << path << ":";
			dynamic_json dj;
			cout << dj.unserialize(js.data()) << endl;
		}
	}

	string js;
	get_file(".//data//pass01.json", js);
	//cout << js.c_str();
	//for (char ch : js)
	//	cout << ch;

	dynamic_json dj2;
	dj2.unserialize(js);
	dj2.dump();
	string ds;
	dj2.dump(ds);
	cout << ds;
	//json_value::vector_helper fv;
	//dj2["statuses"].build_vector_helper(fv);
	//auto &tdj2 = dj2["statuses"][90]["user"];
	//json_value::map_helper mv;
	//
	//PERF(t1, 1000000) {
	//	if(perf_test_t1.count == 1)
	//		dj2["statuses"][90]["user"].build_map_helper(mv);
	//	mv["notifications"];
	//}
}

