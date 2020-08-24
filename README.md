# StaticJson

**fast,easy and static and dynamic**

Many times when we use json as a protocol, we know the definition of each key in advance. When dealing with some high-frequency protocols, we have every reason to define the protocol structure in advance, so that we can get very high parsing speed and Speed of fetching. This is also completely consistent with the design idea of C++, a static language. But to ensure the freedom of the json protocol, the library also supports dynamic expansion of the protocol content at runtime.

�ܶ�ʱ����������json��ΪЭ���ʱ������ǰ֪������ÿ��key�Ķ���ģ��ڴ���һЩ��ƵЭ���ʱ����ȫ�����ɽ�Э��ṹ��ǰ����ã��������Ի�÷ǳ��ߵĽ����ٶȺ�ȡֵ�ٶȣ����C++���ž�̬���Ե����˼·Ҳ����ȫ�Ǻϵġ�����Ϊ�˱�֤jsonЭ������ɶȣ��ÿ�Ҳ֧������ʱ��̬��չЭ�����ݡ�


## Design goals

- **Easy.** You don't need any other third tools, just one header file
- **Fast.** The time complexity of getting the value is O(1) and the time complexity of parsing is O(n)
- **Static define.** You can define the structure like usual, only a tiny difference
- **Dynamic expansion.** You can dynamic expanding the static define --todo
- **STL support.** support the common STL container


- **��.** ����Ҫ�����κε��������ߣ�ֻ��һ��ͷ�ļ�
- **����.** ȡֵ��ʱ�临�ӵ�ΪO(1)��������ʱ�临�Ӷ���O(n)
- **��̬����.** �����������һ�����徲̬�ṹ��ֻ��΢С������
- **��̬��չ.** ����Զ�̬����չ��ľ�̬����
- **��׼��֧��.** ֧�ֳ��õı�׼������

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

�����ǰ֪����json�Ľṹ����Ҫת����C++�ṹ��Ļ������������������Ŀ�����toc.html
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

�� [nativejson-benchmark](https://github.com/miloyip/nativejson-benchmark) ���бȽϸ���json��������ٶȣ�����������ֻ�����������ļ�����һ�β������ٶȶԱȣ������Ŀ�е�ʵ������ǲ����ϵģ���ʵ����Ŀ��һ����ֱȽ϶�Ķ��Ǹ�Ƶ�ʵļ�Э�飬�����Ҿ��û���Ҫ��ѹ�����ԣ��������ܸ��õķ�Ӧ�������Ĺ���/�����ٶȣ������ٶȣ���ȡֵ�ٶȡ�

For simplicity, I will compare with the best-performing rapidjson, my computer��Intel(R) Core(TM)i7-8700 CPU @ 3.20GHz 3.19GHz 64bit 8GB��

Ϊ�˼�,�Ҿͺ����б�����õ�rapidjson���жԱ�,�ҵĵ������ã�Intel(R) Core(TM)i7-8700 CPU @ 3.20GHz 3.19GHz 64bit 8GB��
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

��x64 release �¶Ե���double���ݽ���100000000�ν������Ĺ���/��������������ȡֵ
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

���ǿ��Կ���rapidjson��û���㹻������













