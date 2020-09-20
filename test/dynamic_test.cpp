#pragma once
#include "doctest.h"
#include "static_def.h"
#include "dynamic.h"
#include "perf.h"

extern void get_file(string path, string &res);

TEST_CASE("bool and null test") {
	dynamic_json d;
	d = nullptr;
	CHECK((d == nullptr));

	d = true;
	CHECK((d == true));
}


TEST_CASE("dynamic add test") {
	dynamic_json d;
	d = 123;
	CHECK((d == 123));

	//change type < number size
	d = "hello";
	CHECK((d == "hello"));

	//change type > number size
	d = "hellooooo";
	CHECK((d == "hellooooo"));

	dynamic_json str_d;
	str_d = "hello";
	CHECK((str_d == "hello"));
	//str->num
	str_d = 123;
	CHECK((str_d == 123));

	dynamic_json dj;
	dj["a"] = 123;

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

TEST_CASE("unicode v test") {
	dynamic_json dj;
	dj.unserialize(R({ "a":"\u4f60" }));
	cout << (const char*)dj["a"];
}

TEST_CASE("long key test") {
	string key;
	for (int i = 0; i < 280; i++) {
		key += "a";
	}
	dynamic_json dj;
	dj[key.c_str()] = 1223;

	CHECK((dj[key.c_str()] == 1223));

	string json;
	json += "{\"";
	json += key;
	json += "\":789}";
	dj.unserialize(json);

	CHECK((dj[key.c_str()] == 789));
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

TEST_CASE("serialize test") {
	dynamic_json dj;
	string str;
	dj = 14;
	dj.serialize(str);
	CHECK((str == "14"));

	dj = "hello";
	dj.serialize(str);
	CHECK((str == R("hello")));

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
	dj2["j"] = nullptr;
	dj2["m"] = true;
	dj2["o"] = false;
	dj2.serialize(str);
	CHECK((str == R({"c":{"d":14,"e":"hello"},"d":{"e":"e","b":123},"e":[123,456,789],"f":["a","b"],"k":[{"a":789}],"j":null,"m":true,"o":false})));
}

TEST_CASE("parse test") {
	dynamic_json dj;

	dj.unserialize(R(123));
	CHECK((dj == 123));

	dj.unserialize(R("hello"));
	CHECK((dj == "hello"));

	dj.unserialize(R({"a":2334}));
	CHECK((dj["a"] == 2334));

	dj.unserialize(R({ "a":"hello" }));
	CHECK((dj["a"] == "hello"));

	dj.unserialize(R({ "a": [123,456] }));
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
			dynamic_json dj;
			CHECK(!dj.unserialize(js));
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
			dynamic_json dj;
			CHECK(dj.unserialize(js));
		}
	}

	uint8_t ojf = 255;

	string js;
	get_file(".//data//canada.json", js);
	
	dj.unserialize(R("fsf"));
	const char* jfs = dj;
	dj.unserialize(R(1232));
	int gg= dj;
	string ds;
	//dj2.dump();
	string sff = R({ "d":1.7976931348623157,"f" : "hellllllllllllllllll","c" : {"a":false},"d" : [null,null] });
	PERF(t1, 1) {
		dynamic_json dj2;
		dj2.unserialize(sff);
	}

}

