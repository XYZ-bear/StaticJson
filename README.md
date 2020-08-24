# DynamicJson

**fast,easy and static and dynamic**

�ܶ�ʱ����������json��ΪЭ���ʱ������ǰ֪������ÿ��key�Ķ���ģ�ֻ�������������Ҫ��ȷ����keyֵ���ڴ���һЩ��ƵЭ���ʱ����ȫ�����ɽ�Э��ṹ��ǰ����ã��������Ի�÷ǳ��ߵĽ����ٶȺ�ȡֵ�ٶȣ���̬�������C++���ž�̬���Ե����˼·Ҳ����ȫ�Ǻϵġ�����Ϊ�˱�֤jsonЭ������ɶȣ��ڱ�Ҫ��ʱ��Ҳ��Ҫ̬ͬ�ļ���һЩ���ݡ�


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
test.serialize(R({"a":true,"b":123,"c":123.123,"d":1234.1234,"e":"hello","f":[1,2],"g":["1","2"],"h":{"a":123},"i":[{"a":1},{"a":2}]}));
test.a;
test.h.a;
```
