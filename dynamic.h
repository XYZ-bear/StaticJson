#pragma once

#include "static_json.h"
#include <vector>

/*Linear chain storage structure
Linear: Physical Structure,line memory space,like vector,
Chain: Logical Structure, the nodes linked by logical

In this structure, we can estimate the size of space in advance by the json length, and release the space once time
When visit the nodes, we can visit the brother nodes and child nodes by the head info

|---- | -------|---- | -------|---- | -------|---- | -------
  head	value   head	value   head	value  head	value ......
 -------------- -------------- -------------- --------------
	 node1           node2           node3        node4

head -> |-------------|
			t(char)		the value type number, string, object, array
		|-------------|
		   n(uint32)	the next brother node
		|-------------|
		   cl(uint32)	if t==obj or t==arr the cl mean the first child node, or mean the value length
		|-------------|
		   kl(uint8)	the key length, the default length is 1bit
key ->  |-------------|
			str(kl)
value ->|-------------|
			value
			.......
*/

template <class T>
class iter :public iterator<input_iterator_tag, T>
{
public:
	iter(T * pp,int o) :p(pp),off(o) {}
	iter(const iter<T> &i) :p(i.p) {}
	iter<T> & operator = (const iter<T>& i) { p = i.p; return *this; }
	bool operator != (const iter<T>& i) { 
		if (off == i.off)
			return false;
		return true;
	}
	T & operator *() { return *p; }
	T & operator ++() { 
		off = ++(*p);
		return *p;
	}
private:
	T * p;
	int off;
};

struct json_value {
protected:
	typedef char flag_t;
	typedef uint32_t next_t;
	typedef uint32_t length_t;
	typedef uint64_t number_t;
	typedef uint8_t key_t;
	enum type_flag_t
	{
		emp_t = 0,
		pre_t = 1,
		num_t = 2,
		nul_t = 3,
		boo_t = 4,
		str_t = 5,
		arr_t = 6,
		obj_t = 7,
		del_t = 8
	};

#pragma pack(push,1)
	struct head_t
	{
		flag_t t;
		next_t n;
		length_t cl;
		key_t kl;

		head_t() {
			t = 0;
			kl = 0;
			cl = 0;
			n = 0;
		}

		static inline length_t head_size() {
			return sizeof(head_t);
		}

		static inline length_t no_key_head_size() {
			return sizeof(head_t) - sizeof(key_t);
		}

		bool keycmp(const char* key) {
			const char* kk = (const char*)this + head_size();
			if (!strcmp((const char*)this + head_size(), key)) {
				return true;
			}
			return false;
		}

		const char* get_key() {
			return (const char*)this + head_size();
		}

		char* get_val() {
			return kl == 0 ? (char*)this + head_size() + kl : (char*)this + head_size() + kl + 1;
		}

		template<class N>
		void set_num(N num) {
			*(N*)(get_val()) = num;
		}

		void set_string(const char* str, length_t len) {
			memcpy(get_val(), str, len);
		}

		template<class N>
		N get_num() {
			if (t == type_flag_t::num_t)
				return *(N*)(get_val());
			return 0;
		}

		void get_string(string& str) {
			if (t == type_flag_t::str_t)
				str.assign(get_val(), cl);
		}

		const char* get_string() {
			return get_val();
		}

		bool equal(const char* str) {
			if (memcmp(str, get_val(), cl) == 0)
				return true;
			return false;
		}
	};
#pragma pack(pop)
public:
	json_value() {
	}

	iter<json_value> begin() {
		if (h->t == type_flag_t::obj_t || h->t == type_flag_t::arr_t) {
			h = (head_t*)(data.data() + h->cl);
		}
		return iter<json_value>(this,h->n);
	}

	iter<json_value> end() {
		return iter<json_value>(this, 0);
	}

	json_value& operator [] (const char* key) {
		if (key && h) {
			// set this head as obj_t
			if (h->t == type_flag_t::pre_t) {
				h->t = type_flag_t::obj_t;
				h->cl = data.size();
				h = &push_head(type_flag_t::pre_t, key, strlen(key));
			}
			else {
				// dont find the head -> add this key
				head_t *th = get_key_head(key);
				if (th == nullptr) {
					th = &push_head(type_flag_t::obj_t);
					th->cl = data.size();
					h = &push_head(type_flag_t::pre_t, key, strlen(key));
				}
				else if (h != th) {
					th->n = data.size();
					h = &push_head(type_flag_t::pre_t, key, strlen(key));
				}
			}
		}
		return *this;
	}

	json_value& operator [] (int index) {
		if (index >= 0) {
			// set this head as obj_t
			if (h->t == type_flag_t::pre_t) {
				h->t = type_flag_t::arr_t;
				h->cl = data.size();
				h = &push_head(type_flag_t::pre_t);
			}
			else {
				// dont find the head -> add this key
				head_t *th = get_index_head(index);
				if (th == nullptr) {
					th = &push_head(type_flag_t::arr_t);
					th->cl = data.size();
					h = &push_head(type_flag_t::pre_t);
				}
				else if (h != th) {
					th->n = data.size();
					h = &push_head(type_flag_t::pre_t);
				}
			}
		}
		return *this;
	}

	int operator ++() {
		if (h->n) {
			h = (head_t*)(data.data() + h->n);
			return 1;
		}
		else
			return 0;
	}

	template<class N>
	void operator = (N num) {
		if (h) {
			if (h->t == type_flag_t::num_t)
				h->set_num(num);
			else if (h->t == type_flag_t::pre_t) {
				push_num(num);
			}
		}
	}

	void operator = (const char* str) {
		if (h && str) {
			length_t len = strlen(str);
			if (h->t == type_flag_t::str_t) {
				if (len <= h->cl) {
					h->cl = len;
					h->set_string(str, len + 1);
				}
				else {
					h->t = type_flag_t::del_t;
					h->n = data.size();
					h = &push_head_from(type_flag_t::str_t, h);
					h->cl = len;
					push_str(str, len);
				}
			}
			else if (h->t == type_flag_t::pre_t) {
				h->t = type_flag_t::str_t;
				h->cl = len;
				push_str(str, len);
			}
			else {
				h->t = type_flag_t::del_t;
				h->n = data.size();
				h = &push_head_from(type_flag_t::str_t, h);
				h->cl = len;
				push_str(str, len);
			}
		}
	}

	template<class T>
	operator T()
	{
		return h->get_num<T>();
	}

	operator string() {
		string res;
		h->get_string(res);
		return res;
	}

	operator const char*() {
		return h->get_string();
	}

	bool operator == (const char* str) {
		if (h->t == type_flag_t::str_t)
			return h->equal(str);
		else
			return false;
	}

	template<class N>
	bool operator == (N num) {
		if (h->t == type_flag_t::num_t)
			return h->get_num<N>() == num;
		else
			return false;
	}

	void erase() {
		if (h) {
			h->t = type_flag_t::del_t;
		}
	}

	bool find(const char* key) {
		if (head_t *th = get_key_head(key)) {
			if (th == h)
				return true;
		}
		return false;
	}

	bool is_object() {
		return h->t == type_flag_t::obj_t;
	}

	bool is_array() {
		return h->t == type_flag_t::arr_t;
	}

	bool is_string() {
		return h->t == type_flag_t::str_t;
	}

	bool is_number() {
		return h->t == type_flag_t::num_t;
	}

	bool is_bool() {
		return h->t == type_flag_t::boo_t;
	}

	bool is_null() {
		return h->t == type_flag_t::nul_t;
	}

	size_t size() {
		if (h->t == type_flag_t::arr_t || h->t == type_flag_t::obj_t)
			return count_size();
		return h->cl;
	}

	const char* key() {
		return h->get_key();
	}

	void dump() {
		head_t *th = (head_t*)(data.data() + h->cl);
		const char* kkk = h->get_key();
		bool is_arr = false;
		bool stack = false;
		if (h->t == type_flag_t::obj_t) {
			//	cout << "\"" << th->get_key() << "\":";
			cout << "{";
			stack = true;
		}
		else if (h->t == type_flag_t::arr_t) {
			is_arr = true;
			cout << "[";
		}
		while (th) {
			// if this node was deleted -> jump
			if (th->t != type_flag_t::del_t) {
				// if find the key -> reset the head
				if (th->t == type_flag_t::obj_t || th->t == type_flag_t::arr_t) {
					if (th->kl)
						cout << "\"" << th->get_key() << "\":";
					if (th->cl) {
						h = th;
						dump();
					}
					else {
						if (th->t == obj_t) {
							cout << "{},";
						}
						else {
							cout << "[],";
						}
					}
				}
				if (stack && th->kl)
					cout << "\"" << th->get_key() << "\":";
				if (th->t == type_flag_t::num_t) {
					cout << th->get_num<int>() << ",";
				}
				else if (th->t == type_flag_t::str_t) {
					cout << "\"" << th->get_string() << "\",";
				}
			}
			// return the end head
			if (!th->n) {
				cout << "\b";
				if (stack)
					cout << "},";
				if (is_arr)
					cout << "],";
				//th = (head_t*)(data.data() + th->cl);
				break;
			}
			// point to the brother head
			th = (head_t*)(data.data() + th->n);
		}
	}
private:
	json_value(const json_value &v) {}

	length_t count_size() {
		if (h && h->t == type_flag_t::arr_t && h->cl) {
			// point to the child begin
			head_t *th = (head_t*)(data.data() + h->cl);
			length_t i = 0;
			while (th) {
				// if this node was deleted -> jump
				if (th->t != type_flag_t::del_t) {
					i++;
				}
				// return the end head
				if (!th->n)
					return i;
				// point to the brother head
				th = (head_t*)(data.data() + th->n);

			}
			return i;
		}
		return 0;
	}

	head_t* get_index_head(length_t index) {
		if (h && h->t == type_flag_t::arr_t) {
			// point to the child begin
			head_t *th = (head_t*)(data.data() + h->cl);
			uint32_t i = 0;
			while (th) {
				// if this node was deleted -> jump
				if (th->t != type_flag_t::del_t) {
					// if find the index -> reset the head
					if (i == index) {
						h = th;
						return th;
					}
					i++;
				}
				// return the end head
				if (!th->n)
					return th;
				// point to the brother head
				th = (head_t*)(data.data() + th->n);

			}
			return th;
		}
		return nullptr;
	}

	head_t* get_key_head(const char* key) {
		if (h && h->t == type_flag_t::obj_t) {
			// point to the child begin
			head_t *th = (head_t*)(data.data() + h->cl);
			while (th) {
				// if this node was deleted -> jump
				if (th->t != type_flag_t::del_t) {
					// if find the key -> reset the head
					if (th->keycmp(key)) {
						h = th;
						return th;
					}
				}
				// return the end head
				if (!th->n)
					return th;
				// point to the brother head
				th = (head_t*)(data.data() + th->n);
			}
			return th;
		}
		return nullptr;
	}

protected:
	void init() {
		h = (head_t*)data.data();
	}

	void clear() {
		data.resize(0);
	}

	void push_head_and_set(flag_t t, const char* key, length_t kl) {
		//add head
		length_t off = data.size();
		data.resize(data.size() + head_t::head_size());
		//add key end with '\0'
		push_str(key, kl);

		h = (head_t*)(data.data() + off);
		h->t = t;
		h->kl = kl;
	}

	void push_head_and_set(flag_t t) {
		//add head
		length_t off = data.size();
		data.resize(data.size() + head_t::head_size());
		//add key end with '\0'

		h = (head_t*)(data.data() + off);
		h->t = t;
	}

	void set_next(int off) {
		head_t *th = (head_t*)(data.data() + off);
		th->n = data.size();
	}

	void set_next() {
		h->n = data.size();
	}

	void set_empty_next() {
		h->n = 0;
	}

	void set_head(int off) {
		h = (head_t*)(data.data() + off);
	}

	void set_empty_next(int off) {
		h = (head_t*)(data.data() + off);
		h->n = 0;
	}

	void set_child_or_length() {
		h->cl = data.size();
	}

	flag_t get_flag() {
		return h->t;
	}

	int get_off() {
		return (const char*)h - data.data();
	}

	size_t get_data_size() {
		return data.size();
	}

	void set_empty_child_or_length() {
		h->cl = 0;
	}

	head_t& push_head(flag_t t) {
		//add head
		length_t off = data.size();
		data.resize(data.size() + head_t::head_size());
		head_t &head = *(head_t*)(data.data() + off);
		head.t = t;
		return head;
	}

	head_t& push_head(flag_t t, const char* key, length_t kl) {
		//add head
		length_t off = data.size();
		data.resize(data.size() + head_t::head_size());
		//add key end with '\0'
		push_str(key, kl);

		head_t &head = *(head_t*)(data.data() + off);
		head.t = t;
		head.kl = kl;
		return head;
	}

	void set_flag(char f) {
		h->t = f;
		h->cl = data.size();
	}

	head_t& push_head_from(flag_t t, head_t* from) {
		length_t old_off = (const char*)from - data.data();
		//add head
		length_t off = data.size();
		data.resize(data.size() + head_t::head_size());
		//add key end with '\0'
		from = (head_t*)(data.data() + old_off);
		length_t kl = from->kl;
		push_str(from->get_key(), kl);
		//cur head
		head_t &head = *(head_t*)(data.data() + off);
		//old_head
		head.t = t;
		head.kl = kl;
		return head;
	}

	inline void push_str(const char* str, length_t len) {
		data.append(str, len);
		data.append(1, '\0');
	}

	inline void push_str_and_set(const char* str, length_t len) {
		h->t = type_flag_t::str_t;
		h->cl = len;// data.size();
		length_t head_off = (const char*)h - data.data();
		data.append(str, len);
		data.append(1, '\0');
		h = (head_t*)(data.data() + head_off);
	}

	template<class N>
	inline N& push_num() {
		h->t = type_flag_t::num_t;
		length_t head_off = (const char*)h - data.data();
		data.resize(data.size() + sizeof(number_t));
		h = (head_t*)(data.data() + head_off);
		//h->set_int(0);
		return *(N*)h->get_val();
	}

	template<class N>
	inline void push_num(N num) {
		h->t = type_flag_t::num_t;
		length_t head_off = (const char*)h - data.data();
		data.resize(data.size() + sizeof(number_t));
		h = (head_t*)(data.data() + head_off);
		h->set_num(num);
	}

private:
	string data;
	head_t* h;
};



#define ERROR_RETURT(pos) cout << "[error]:" << pos << endl;return false;


class dynamic_json :public json_value {
public:
	dynamic_json() {
	}
	~dynamic_json() {
	}
private:
	struct info
	{
		bool is_obj;
		int off;
	};

	class mystack {
	public:
		vector<info> vec;
		mystack() {
			vec.reserve(50);
		}
		void push(const info& in) {
			vec.emplace_back(in);
		}
		info& top() {
			return vec.back();
		}
		void pop() {
			vec.pop_back();
		}
		size_t size() {
			return vec.size();
		}
	};

	//Non recursive implementation, so there is no limit on the depth, it is up on your memory size
	//nested grammer
	//obj:{ -> "key" -> : -> value -> }
	//arr:[ -> value -> ]
	bool parse(json_stream &js) {
		mystack stack;
		parser::skip_space(js);

		while (char ch = parser::get_cur_and_next(js)) {
			//step 1: check start tokens , [ {
			//--------------------------------------------------------
			parser::skip_space(js);
			// , end and begin token
			if (ch == parser::json_key_symbol::next_key_value) {
				set_next();
			}
			else if (ch == parser::json_key_symbol::object_begin) {
				if (get_flag() == type_flag_t::pre_t)
					set_flag(type_flag_t::obj_t);
				else {
					push_head_and_set(type_flag_t::obj_t);
					set_child_or_length();
				}
				stack.push({ true,(int)get_off() });
			}
			else if (ch == parser::json_key_symbol::array_begin) {
				//[], empty array
				if (*js.begin == parser::json_key_symbol::array_end) {
					parser::get_next(js);
					parser::skip_space(js);
					set_flag(type_flag_t::arr_t);
					set_empty_child_or_length();
					continue;
				}
				//[x,x,x] and [],[],[]
				if (get_flag() == type_flag_t::pre_t)
					set_flag(type_flag_t::arr_t);
				else {
					push_head_and_set(type_flag_t::arr_t);
					set_child_or_length();
				}
				stack.push({ false,(int)get_off() });
			}
			else if (ch == parser::json_key_symbol::object_end) {
				if (stack.size() > 0 && stack.top().is_obj) {
					set_head(stack.top().off);
					stack.pop();
				}
				else {
					ERROR_RETURT(js.begin);
				}
				parser::skip_space(js);
				continue;
			}
			else if (ch == parser::json_key_symbol::array_end) {
				if (stack.size() > 0 && !stack.top().is_obj) {
					set_head(stack.top().off);
					stack.pop();
				}
				else {
					ERROR_RETURT(js.begin);
				}
				parser::skip_space(js);
				continue;
			}
			if (stack.size() == 0) {
				ERROR_RETURT(js.begin);
			}

			//step 2: parse key
			//-----------------------------------------------------------
			//{ "x":x } obj head with key ,or [x,x,x] arr head without key 
			parser::skip_space(js);
			if (stack.top().is_obj) {
				if (ch = parser::get_cur_and_next(js)) {
					//key must start with quote
					if (ch == parser::json_key_symbol::str) {
						//push head
						const char* b = js.begin;
						push_head_and_set(type_flag_t::pre_t, b, parser::skip_str(js));

						//check key_value separator
						parser::skip_space(js);
						if (parser::get_cur_and_next(js) != parser::json_key_symbol::key_value_separator) {
							ERROR_RETURT(js.begin);
						}
					}
					//if no value pop stack
					else if (ch == parser::json_key_symbol::object_end) {
						set_empty_child_or_length();
						stack.pop();
						continue;
					}
					else {
						//error format
						ERROR_RETURT(js.begin);
					}
				}
				else {
					//error format
					ERROR_RETURT(js.begin);
				}
			}
			else {
				push_head_and_set(type_flag_t::pre_t);
			}

			//step 3: parse value
			//-----------------------------------------------------------
			parser::skip_space(js);
			if (char ch = *js.begin) {
				if (ch == parser::json_key_symbol::str) {
					parser::get_next(js);
					const char* b = js.begin;
					push_str_and_set(b, parser::skip_str(js));
				}
				else if (ch == parser::json_key_symbol::object_begin) {
					continue;
				}
				else if (ch == parser::json_key_symbol::array_begin) {
					continue;
				}
				else {
					if (ch == 'f') {
						js.begin += 5;
						push_num(false);
						set_flag(type_flag_t::boo_t);
					}
					else if(ch == 't'){
						js.begin += 4;
						push_num(true);
						set_flag(type_flag_t::boo_t);
					}
					else if (ch == 'n') {
						js.begin += 4;
						set_flag(type_flag_t::nul_t);
					}
					else {
						if (parser::is_double(js))
							parser::unserialize(&push_num<double>(), js);
						else
							parser::unserialize_int(&push_num<uint64_t>(), js);
					}
				}
			}
			parser::skip_space(js);

			//step 4: check end tokens
			//-----------------------------------------------------------
			//check end token only } ] , is allowed
			if (*js.begin == parser::json_key_symbol::next_key_value || *js.begin == parser::json_key_symbol::object_end || *js.begin == parser::json_key_symbol::array_end) {
				continue;
			}

			ERROR_RETURT(js.begin);
			//-----------------------------------------------------------
		}

		if (stack.size() == 0)
			return true;
		return false;
	}
public:
	//json_value json_data;
	json_value & operator [] (const char* key) {
		init();
		return json_value::operator[](key);
	}

	json_value& operator [] (int index) {
		init();
		return json_value::operator[](index);
	}

	void dump() {
		init();
		return json_value::dump();
	}
	// if *json_value end with '\0',don't need the size arg
	size_t unserialize(const char* json, size_t size = 0) {
		//data.resize(0);
		clear();
		init();
		const char* begin = json;
		const char* end = nullptr;
		if (size > 0)
			end = begin + size;
		json_stream js{ begin,end };
		if (parse(js))
			return js.begin - json;
		return 0;
	}
};