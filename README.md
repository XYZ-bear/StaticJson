# StaticJson

**fast,easy and static and dynamic**

Many times when we use json as a protocol, we know the definition of each key in advance. When dealing with some high-frequency protocols, we have every reason to define the protocol structure in advance, so that we can get very high parsing speed and Speed of fetching. This is also completely consistent with the design idea of C++, a static language. But to ensure the freedom of the json protocol, the library also supports dynamic expansion of the protocol content at runtime.

很多时候我们在用json作为协议的时候都是提前知道其中每个key的定义的，在处理一些高频协议的时候，完全有理由将协议结构提前定义好，这样可以获得非常高的解析速度和取值速度，这和C++这门静态语言的设计思路也是完全吻合的。但是为了保证json协议的自由度，该库也支持运行时动态拓展协议内容。


## Design goals

- **Easy.** You don't need any other third tools, just one header file
- **Fast.** The time complexity of getting the value is O(1) and the time complexity of parsing is O(n)
- **Static define.** You can define the structure like usual, only a tiny difference
- **Dynamic expansion.** You can dynamic expanding the static define --todo
- **STL support.** support the common STL container


- **简单.** 不需要依赖任何第三方工具，只有一个头文件
- **快速.** 取值的时间复杂的为O(1)，解析的时间复杂度是O(n)
- **静态定义.** 你可以像往常一样定义静态结构，只有微小的区别
- **动态拓展.** 你可以动态的拓展你的静态定义
- **标准库支持.** 支持常用的标准库容器

##Examples

static define
```c
#include "static_json.h>

Json(Test){
public:
	bool N(a);
	int N(b);
	double N(c);
	float N(d);
	string N(e);
	vector<int> N(f);
	vector<string> N(g);
	Json(Test2){
	public:
		int N(a);
	};
	Test2 N(h);
	vector<Test2> N(i);
};
```
usage
```c
	Test test;
	test.unserialize(R({"a":true,"b":123,"c":123.123,"d":1234.1234,"e":"hello","f":[1,2],"g":["1","2"],"h":{"a":123},"i":[{"a":1},{"a":2}]}));
	test.a;
	test.h.a;
	string res;
	test.serialize(res);
```
##json to C++/C

If you know the structure of json in advance and want to convert to C++ structure, you can open toc.html in the project with a browser

如果提前知道了json的结构，想要转换到C++结构体的话，可以用浏览器打开项目里面的toc.html
```c
{
    "req_time": "2020-08-05 01:50:31",
    "req_body": {
        "idNumber": [
            "999993221223912",
            "231313131112412"
        ],
        "mobile": [
            "13131311124",
            "13131112412"
        ],
        "driverLicenseNumber": [
            "144142223",
            "142223131"
        ]
    }
}
```
```c
Json(Test)
{
public:
    string N(req_time);
    Json(req_body_t){
    public:
        vector<string> N(idNumber);
        vector<string> N(mobile);
        vector<string> N(driverLicenseNumber);
    };
    req_body_t N(req_body);
};
```

##Compare

In [nativejson-benchmark](https://github.com/miloyip/nativejson-benchmark), there is a comparison of the speed of various json parsing libraries, but the test only parse the three large files once and compare the speeds. This is inconsistent with the actual situation in the project, in actual projects, there are generally more simple protocols with high frequency, so I think there is a need for stress testing, so as to better reflect the construction/destruction speed, analysis speed, and value speed of the parser.

在 [nativejson-benchmark](https://github.com/miloyip/nativejson-benchmark) 中有比较各种json解析库的速度，不过看了下只对那三个大文件解析一次并进行速度对比，这和项目中的实际情况是不符合的，在实际项目中一般出现比较多的都是高频率的简单协议，所以我觉得还需要有压力测试，这样才能更好的反应解析器的构造/析构速度，解析速度，和取值速度。

For simplicity, I will compare with the best-performing rapidjson, my computer（Intel(R) Core(TM)i7-8700 CPU @ 3.20GHz 3.19GHz 64bit 8GB）

为了简单,我就和其中表现最好的rapidjson进行对比,我的电脑配置（Intel(R) Core(TM)i7-8700 CPU @ 3.20GHz 3.19GHz 64bit 8GB）
```c
#include <map>
#include <functional>
#include <time.h>
#include <string>
#include <iostream>
using namespace std;
#define R(...) #__VA_ARGS__

void perf_test(string name, size_t t, function<void()> f) {
	clock_t startTime, endTime;
	startTime = clock();
	for (size_t i = 0; i < t; i++)
		f();
	endTime = clock();
	cout << name << ":" << endTime - startTime << "ms" << endl;
}
```
Under x64 release, a single double data will be constructed/destructed, parsed, and valued 100000000 times

在x64 release 下对单个double数据进行100000000次解析器的构造/析构，解析，和取值
```c
perf_test("perf test", 100000000, []()->void {
	Document d;
	d.Parse(R({ "d":123.34545435345534534 }));
	double res = d["d"].GetDouble();
});
```
> perf test:55672ms

```c
Json(Float) {
public:
	double N(d);
};

perf_test("perf test", 100000000, []()->void {
	Float f;
	f.unserialize(R({"d":123.34545435345534534}));
	double res = f.d;
});
```
> perf test:19737ms

我们可以看到rapidjson并没有足够的优秀













