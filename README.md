# DynamicJson

**fast,easy and static and dynamic**

很多时候我们在用json作为协议的时候都是提前知道其中每个key的定义的，只有少数需求才需要不确定的key值，在处理一些高频协议的时候，完全有理由将协议结构提前定义好，这样可以获得非常高的解析速度和取值速度，静态定义这和C++这门静态语言的设计思路也是完全吻合的。但是为了保证json协议的自由度，在必要的时候也需要同态的加入一些内容。


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
test.serialize(R({"a":true,"b":123,"c":123.123,"d":1234.1234,"e":"hello","f":[1,2],"g":["1","2"],"h":{"a":123},"i":[{"a":1},{"a":2}]}));
test.a;
test.h.a;
```
