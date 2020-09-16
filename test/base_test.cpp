#pragma once
#include "doctest.h"
#include "static_json.h"
#include "perf.h"

TEST_CASE("no_copy_string test") {
	hash<no_copy_string> h;
	CHECK(h(no_copy_string("")) == 0);
	CHECK(h(no_copy_string("\"")) == 0);
	CHECK(h(no_copy_string("\\\"")) != 0);
	CHECK(h(no_copy_string("123")) == h(no_copy_string("123")));
	CHECK(h(no_copy_string("123")) != h(no_copy_string("123123")));
	CHECK(h(no_copy_string("123")) == h(no_copy_string("123\"")));
	CHECK(h(no_copy_string("123\\")) != h(no_copy_string("123\\\"")));
	CHECK(h(no_copy_string("123\\")) != h(no_copy_string("123\\\"")));

	equal_to<no_copy_string> e;
	CHECK(!e(no_copy_string("adc"), no_copy_string("abcd")));
	CHECK(!e(no_copy_string("adc"), no_copy_string("bcd")));
	CHECK(e(no_copy_string("abc"), no_copy_string("abc\"sfsdf")));
	CHECK(!e(no_copy_string("abc\\"), no_copy_string("abc\\\"")));///  abc\ != abc\"
	CHECK(e(no_copy_string("abc\\\""), no_copy_string("abc\\\"\"")));///  abc\" == abc\""
}
