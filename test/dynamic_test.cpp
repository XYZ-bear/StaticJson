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
	CHECK((dj.find("c") == dj.end()));
	
}

TEST_CASE("dynamic insert test") {
	dynamic_json dj;
	dj.insert(1, 2);
	dj[3].insert(3, 4);
	dj[3].insert(5, 6);
	dj.insert(7, 8);

	CHECK((dj[1] == 2));
	CHECK((dj[3][3] == 4));
	CHECK((dj[3][5] == 6));
	CHECK((dj[7] == 8));

	dj[3].erase();

	dj.insert(8, 10);
	dj.insert(11, 12);

	CHECK((dj[8] == 10));
	CHECK((dj[11] == 12));
	CHECK((dj.find(3) == dj.end()));
}

TEST_CASE("unicode v test") {
	dynamic_json dj;
	dj.unserialize(R({ "a":"\u4f60" }));
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

	dj["a"] = size_t(18446744073709551615);
	CHECK((dj["a"] == size_t(18446744073709551615)));

	dj.unserialize(R({ "a": 18446744073709551615 }));
	CHECK((dj["a"] == size_t(18446744073709551615)));

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
	dj2["e"].push_back(123);
	dj2["e"].push_back(456);
	dj2["f"].push_back("a");
	dj2["f"].push_back("b");
	dj2["e"].push_back(789);
	dj2["j"] = nullptr;
	dj2["m"] = true;
	dj2["o"] = false;
	dj2.serialize(str);
	CHECK((str == R({"c":{"d":14,"e":"hello"},"d":{"e":"e","b":123},"e":[123,456,789],"f":["a","b"],"j":null,"m":true,"o":false})));
}


TEST_CASE("invalid number test") {
	dynamic_json n;
	
	CHECK(!n.unserialize(R(01)));
	CHECK(!n.unserialize(R(--1)));
	CHECK(!n.unserialize(R(1.)));
	CHECK(!n.unserialize(R(1E)));
	CHECK(!n.unserialize(R(1E- )));
	CHECK(!n.unserialize(R(1.E1)));
	CHECK(!n.unserialize(R(-1E)));
	CHECK(!n.unserialize(R(-0E#)));
	CHECK(!n.unserialize(R(-0E-#)));
	CHECK(!n.unserialize(R(-0#)));
	CHECK(!n.unserialize(R(-0.0:)));
	CHECK(!n.unserialize(R(-0.0Z)));
	CHECK(!n.unserialize(R(-0E123:)));
	CHECK(!n.unserialize(R(-0e0-:)));
	CHECK(!n.unserialize(R(-0e-:)));
	CHECK(!n.unserialize(R(-0f)));
	CHECK(!n.unserialize(R(+1)));
	CHECK(!n.unserialize(R(+0)));
}

TEST_CASE("string test") {
	dynamic_json n;
	// empty string
	CHECK(n.unserialize(R("")));

	// error: tab in string
	CHECK(n.unserialize("\"\t\"") == false);
	// error: newline in string
	CHECK(n.unserialize("\"\n\"") == false);
	CHECK(n.unserialize("\"\r\"") == false);
	// error: backspace in string
	CHECK(n.unserialize("\"\b\"") == false);
	// improve code coverage
	CHECK(n.unserialize("\uFF01") == false);
	CHECK(n.unserialize("[-4:1,]") == false);
	// unescaped control characters
	CHECK(n.unserialize("\"\x00\"") == false);
	CHECK(n.unserialize("\"\x01\"") == false);
	CHECK(n.unserialize("\"\x02\"") == false);
	CHECK(n.unserialize("\"\x03\"") == false);
	CHECK(n.unserialize("\"\x04\"") == false);
	CHECK(n.unserialize("\"\x05\"") == false);
	CHECK(n.unserialize("\"\x06\"") == false);
	CHECK(n.unserialize("\"\x07\"") == false);
	CHECK(n.unserialize("\"\x08\"") == false);
	CHECK(n.unserialize("\"\x09\"") == false);
	CHECK(n.unserialize("\"\x0a\"") == false);
	CHECK(n.unserialize("\"\x0b\"") == false);
	CHECK(n.unserialize("\"\x0c\"") == false);
	CHECK(n.unserialize("\"\x0d\"") == false);
	CHECK(n.unserialize("\"\x0e\"") == false);
	CHECK(n.unserialize("\"\x0f\"") == false);
	CHECK(n.unserialize("\"\x10\"") == false);
	CHECK(n.unserialize("\"\x11\"") == false);
	CHECK(n.unserialize("\"\x12\"") == false);
	CHECK(n.unserialize("\"\x13\"") == false);
	CHECK(n.unserialize("\"\x14\"") == false);
	CHECK(n.unserialize("\"\x15\"") == false);
	CHECK(n.unserialize("\"\x16\"") == false);
	CHECK(n.unserialize("\"\x17\"") == false);
	CHECK(n.unserialize("\"\x18\"") == false);
	CHECK(n.unserialize("\"\x19\"") == false);
	CHECK(n.unserialize("\"\x1a\"") == false);
	CHECK(n.unserialize("\"\x1b\"") == false);
	CHECK(n.unserialize("\"\x1c\"") == false);
	CHECK(n.unserialize("\"\x1d\"") == false);
	CHECK(n.unserialize("\"\x1e\"") == false);
	CHECK(n.unserialize("\"\x1f\"") == false);


	// quotation mark "\""
	CHECK(n.unserialize("\"\\\"\""));
	// reverse solidus "\\"
	CHECK(n.unserialize("\"\\\\\""));
	// solidus
	CHECK(n.unserialize("\"\\/\""));
	// backspace
	CHECK(n.unserialize("\"\\b\""));
	// formfeed
	CHECK(n.unserialize("\"\\f\""));
	// newline
	CHECK(n.unserialize("\"\\n\""));
	// carriage return
	CHECK(n.unserialize("\"\\r\""));
	// horizontal tab
	CHECK(n.unserialize("\"\\t\""));

	CHECK(n.unserialize("\"\\u0001\""));
	CHECK(n.unserialize("\"\\u000a\""));
	CHECK(n.unserialize("\"\\u00b0\""));
	CHECK(n.unserialize("\"\\u0c00\""));
	CHECK(n.unserialize("\"\\ud000\""));
	CHECK(n.unserialize("\"\\u000E\""));
	CHECK(n.unserialize("\"\\u00F0\""));
	CHECK(n.unserialize("\"\\u0100\""));
	CHECK(n.unserialize("\"\\u2000\""));
	CHECK(n.unserialize("\"\\uFFFF\""));
	CHECK(n.unserialize("\"\\u20AC\""));
	CHECK(n.unserialize("\"€\""));
	CHECK(n.unserialize("\"🎈\""));

	CHECK(n.unserialize("\"\\ud80c\\udc60\""));
	CHECK(n.unserialize("\"\\ud83c\\udf1e\""));
}

TEST_CASE("true false null test") {
	dynamic_json n;
	CHECK(!n.unserialize("n"));
	CHECK(!n.unserialize("nu"));
	CHECK(!n.unserialize("nul"));
	CHECK(!n.unserialize("nulk"));
	CHECK(!n.unserialize("nulm"));

	CHECK(!n.unserialize("t"));
	CHECK(!n.unserialize("tr"));
	CHECK(!n.unserialize("tru"));
	CHECK(!n.unserialize("trud"));
	CHECK(!n.unserialize("truf"));

	CHECK(!n.unserialize("f"));
	CHECK(!n.unserialize("fa"));
	CHECK(!n.unserialize("fal"));
	CHECK(!n.unserialize("fals"));
	CHECK(!n.unserialize("falsd"));
	CHECK(!n.unserialize("falsf"));
}

TEST_CASE("int key") {
	dynamic_json n;
	n.insert(-1, 2);
	CHECK((n[-1] == 2));

	n.insert(1, 3);
	CHECK((n[1] == 3));

	n.insert(1.1, 4);
	CHECK((n[1.1] == 4));

	// n.insert(true, 5);
	// CHECK((n[true] == 5));

	for (int i = 10; i < 100; i++) {
		n.insert(i, i);
	}

	for (int i = 10; i < 100; i++) {
		CHECK((n[i] == i));
	}

	dynamic_json n2;
	n2.pre_allocate(200);
	for (int i = 101; i < 200; i++) {
		n2[i] = i;
	}

	for (int i = 101; i < 200; i++) {
		CHECK((n2[i] == i));
	}
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

	auto aa = dj["a"];
	CHECK((aa[0] == 123));
	CHECK((aa[1] == 456));

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
	CHECK((dj["a"].size() == 0));
	CHECK((dj["b"].size() == 0));

	CHECK(dj.unserialize(R({])) == 0);
	CHECK(dj.unserialize(R({"":12})));

	CHECK(dj.unserialize(R({ "a": [{"b":8},234,"abc",[123]] }}}})) == 0);
	CHECK(dj.unserialize(R(,"a": [{"b":8},234,"abc",[123]] }}}})) == 0);

	CHECK(dj.unserialize(R({ "a": [[[1,2],[3,4,5]],[[1,2],[3,4,5]]] })));

	string path = ".//data//YhSwC2lEySnxeycw.json";
	string js;
	get_file(path, js);
	PERF(42, 1) {
		

		if (js.size() > 0) {
			dynamic_json dj;
			dj.unserialize(js, UNESCAPE_UNICODE | UNESCAPE);

		
			// cout << dj.size() << endl;
			// string f;
			// dj.serialize(f);
			// //cout << f << endl;
			// ofstream file;
			// file.open("YhSwC2lEySnxeycw2.json");
			// file << f;
		}

	}

	dynamic_json dj2;
	auto f = dj2["a"];
	f.push_back(1);
	f.push_back("a");

	auto aaa = dj2.mutl_obj("aaa");
	auto bbb = aaa.mutl_arr();
	bbb.push_back(1); 
	auto ccc = bbb.mutl_arr();
	ccc.push_back(2);
	ccc.push_back(4);

	auto ccc2 = bbb.mutl_arr();
	ccc2.push_back(21);
	ccc2.push_back(41);

	string res;
	dj2.serialize(res);
	cout<< "------------" << res<<endl;



	// auto bbb = dj2.mutl_arr();
	// bbb.push_back(1);

	return;

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



}

