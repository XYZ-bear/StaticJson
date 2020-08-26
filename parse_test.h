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
    Json(query_t){
    public:
        int N(count);
        string N(created);
        string N(lang);
        Json(results_t){
        public:
            Json(channel_t){
            public:
                Json(units_t){
                public:
                    string N(distance);
                    string N(pressure);
                    string N(speed);
                    string N(temperature);
                };
                units_t N(units);
                string N(title);
                string N(link);
                string N(description);
                string N(language);
                string N(lastBuildDate);
                string N(ttl);
                Json(location_t){
                public:
                    string N(city);
                    string N(country);
                    string N(region);
                };
                location_t N(location);
                Json(wind_t){
                public:
                    string N(chill);
                    string N(direction);
                    string N(speed);
                };
                wind_t N(wind);
                Json(atmosphere_t){
                public:
                    string N(humidity);
                    string N(pressure);
                    string N(rising);
                    string N(visibility);
                };
                atmosphere_t N(atmosphere);
                Json(astronomy_t){
                public:
                    string N(sunrise);
                    string N(sunset);
                };
                astronomy_t N(astronomy);
                Json(image_t){
                public:
                    string N(title);
                    string N(width);
                    string N(height);
                    string N(link);
                    string N(url);
                };
                image_t N(image);
                Json(item_t){
                public:
                    string N(title);
                    string N(lat);
                    string N(longg);
                    string N(link);
                    string N(pubDate);
                    Json(condition_t){
                    public:
                        string N(code);
                        string N(date);
                        string N(temp);
                        string N(text);
                    };
                    condition_t N(condition);
                    Json(forecast_t){
                    public:
                        string N(code);
                        string N(date);
                        string N(day);
                        string N(high);
                        string N(low);
                        string N(text);
                    };
                    vector<forecast_t> N(forecast);
                    string N(description);
                    Json(guid_t){
                    public:
                        string N(isPermaLink);
                    };
                    guid_t N(guid);
                };
                item_t N(item);
            };
            channel_t N(channel);
        };
        results_t N(results);
    };
    query_t N(query);
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

Json(TI) {
public:
	int N(a);
	string N(b);
	vector<int> N(c);
};

Json(Canada)
{
public:
    string N(type);
    Json(features_t){
    public:
        string N(type);
        Json(properties_t){
        public:
            string N(name);
        };
        properties_t N(properties);
        Json(geometry_t){
        public:
            string N(type);
            vector<vector<vector<double>>> N(coordinates);
        };
        geometry_t N(geometry);
    };
    vector<features_t> N(features);
};

Json(Float) {
public:
	double N(d);
};
#include "twitter.h"
TEST_CASE("twitter test") {
	ifstream myfile("C:\\Users\\xiongyanzhong01\\Downloads\\nativejson-benchmark-master\\data\\twitter.json");
	string res;
	string temp;
	while (getline(myfile, temp))
	{
		res += temp;
	}
	myfile.close();

	perf_test("table build 1", 10000, [&res]()->void {
		Twitter twitter;
		twitter.unserialize(res.data());
		string s;
		twitter.serialize(s);
		//Float f;
		//f.unserialize(R({ "d":123.12321321321332 }));
	});
	getchar();
}
//
//TEST_CASE("complex test") {
//	ifstream myfile("C:\\Users\\xiongyanzhong01\\Downloads\\nativejson-benchmark-master\\data\\canada.json");
//	string res;
//	string temp;
//	while (getline(myfile, temp))
//	{
//		res += temp;
//	}
//	myfile.close();
//
//
//	perf_test("table build 1", 1000, [&res]()->void {
//		Canada canada;
//		canada.unserialize(res.data());
//		string s;
//		canada.serialize(s);
//		//Float f;
//		//f.unserialize(R({ "d":123.12321321321332 }));
//	});
//
//
//
//	perf_test("table build 1", 200000000, []()->void {
//		TI ti;
//		ti.unserialize(R({"a",123,"b":"hello","c":[12,34]}));
//	});
//	perf_test("table build 1", 1200000, []()->void {
//		Complex t;
//
//		t.unserialize(R(
//			{
//				"query": {
//					"count": 1,
//					"created" : "2017-10-18T14:34:38Z",
//					"lang" : "zh-CN",
//					"results" : {
//						"channel": {
//							"units": {
//								"distance": "mi",
//								"pressure" : "in",
//								"speed" : "mph",
//								"temperature" : "F"
//							},
//							"title": "Yahoo! Weather - Beijing, Beijing, CN",
//							"link" : "http://us.rd.yahoo.com/dailynews/rss/weather/Country__Country/*https://weather.yahoo.com/country/state/city-2151330/",
//							"description" : "Yahoo! Weather for Beijing, Beijing, CN",
//							"language" : "en-us",
//							"lastBuildDate" : "Wed, 18 Oct 2017 10:34 PM CST",
//							"ttl" : "60",
//							"location" : {
//								"city": "Beijing",
//								"country" : "China",
//								"region" : " Beijing"
//							},
//							"wind" : {
//								"chill": "54",
//								"direction" : "225",
//								"speed" : "4"
//							},
//							"atmosphere" : {
//								"humidity": "95",
//								"pressure" : "1018.0",
//								"rising" : "0",
//								"visibility" : "10.7"
//							},
//							"astronomy": {
//								"sunrise": "6:28 am",
//								"sunset" : "5:30 pm"
//							},
//							"image" : {
//								"title": "Yahoo! Weather",
//								"width" : "142",
//								"height" : "18",
//								"link" : "http://weather.yahoo.com",
//								"url" : "http://l.yimg.com/a/i/brand/purplelogo//uh/us/news-wea.gif"
//							},
//							"item": {
//								"title": "Conditions for Beijing, Beijing, CN at 09:00 PM CST",
//								"lat" : "39.90601",
//								"long" : "116.387909",
//								"link" : "http://us.rd.yahoo.com/dailynews/rss/weather/Country__Country/*https://weather.yahoo.com/country/state/city-2151330/",
//								"pubDate" : "Wed, 18 Oct 2017 09:00 PM CST",
//								"condition" : {
//									"code": "29",
//									"date" : "Wed, 18 Oct 2017 09:00 PM CST",
//									"temp" : "54",
//									"text" : "Partly Cloudy"
//								},
//								"forecast": [
//									{
//										"code": "12",
//										"date" : "18 Oct 2017",
//										"day" : "Wed",
//										"high" : "57",
//										"low" : "51",
//										"text" : "Rain"
//									},
//									{
//										"code": "30",
//										"date" : "19 Oct 2017",
//										"day" : "Thu",
//										"high" : "64",
//										"low" : "48",
//										"text" : "Partly Cloudy"
//									},
//									{
//										"code": "30",
//										"date" : "20 Oct 2017",
//										"day" : "Fri",
//										"high" : "66",
//										"low" : "49",
//										"text" : "Partly Cloudy"
//									},
//									{
//										"code": "39",
//										"date" : "21 Oct 2017",
//										"day" : "Sat",
//										"high" : "65",
//										"low" : "52",
//										"text" : "Scattered Showers"
//									},
//									{
//										"code": "39",
//										"date" : "22 Oct 2017",
//										"day" : "Sun",
//										"high" : "53",
//										"low" : "46",
//										"text" : "Scattered Showers"
//									},
//									{
//										"code": "30",
//										"date" : "23 Oct 2017",
//										"day" : "Mon",
//										"high" : "59",
//										"low" : "44",
//										"text" : "Partly Cloudy"
//									},
//									{
//										"code": "34",
//										"date" : "24 Oct 2017",
//										"day" : "Tue",
//										"high" : "62",
//										"low" : "42",
//										"text" : "Mostly Sunny"
//									},
//									{
//										"code": "30",
//										"date" : "25 Oct 2017",
//										"day" : "Wed",
//										"high" : "61",
//										"low" : "45",
//										"text" : "Partly Cloudy"
//									},
//									{
//										"code": "30",
//										"date" : "26 Oct 2017",
//										"day" : "Thu",
//										"high" : "62",
//										"low" : "48",
//										"text" : "Partly Cloudy"
//									},
//									{
//										"code": "30",
//										"date" : "27 Oct 2017",
//										"day" : "Fri",
//										"high" : "63",
//										"low" : "45",
//										"text" : "Partly Cloudy"
//									}
//								],
//								"description": "<![CDATA[<img src=\"http://l.yimg.com/a/i/us/we/52/29.gif\"/>\n<BR />\n<b>Current Conditions:</b>\n<BR />Partly Cloudy\n<BR />\n<BR />\n<b>Forecast:</b>\n<BR /> Wed - Rain. High: 57Low: 51\n<BR /> Thu - Partly Cloudy. High: 64Low: 48\n<BR /> Fri - Partly Cloudy. High: 66Low: 49\n<BR /> Sat - Scattered Showers. High: 65Low: 52\n<BR /> Sun - Scattered Showers. High: 53Low: 46\n<BR />\n<BR />\n<a href=\"http://us.rd.yahoo.com/dailynews/rss/weather/Country__Country/*https://weather.yahoo.com/country/state/city-2151330/\">Full Forecast at Yahoo! Weather</a>\n<BR />\n<BR />\n<BR />\n]]>",
//								"guid" : {
//									"isPermaLink": "false"
//								}
//							}
//						}
//					}
//				}
//			}));
//	});
//	perf_test("table build 1", 1000000, []()->void {
//		Complex2 t;
//	});
//	//getchar();
//}

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