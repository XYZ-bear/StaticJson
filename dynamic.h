#pragma once

#include "static_json.h"
#include <vector>

class dynamic_json :public parser {
public:
	typedef char flag_t;
	typedef uint32_t next_t;
	typedef uint32_t length_t;
	typedef double number_t;
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
	dynamic_json() {
		//json_data.reserve(1024);
	}
	~dynamic_json() {
	}

	bool parse_key_value(const char** begin, const char* end) {
		parser::skip_space(begin, end);
		if (parser::get_cur_and_next(begin, end) == parser::json_key_symbol::str) {
			const char* b = *begin;
			parser::skip_str(begin, end);
			int len = *begin - b - 1;
			parser::skip_space(begin, end);
			if (parser::get_cur_and_next(begin, end) == parser::json_key_symbol::key_value_separator) {
				json_data.push_head_and_set(type_flag_t::pre_t, b, len);
				return unserialize(begin, end);
			}
			else
				return false;
		}
		return false;
	}

	bool unserialize(const char** begin, const char* end) {
		skip_space(begin, end);
		if (char ch = **begin) {
			if (ch == parser::json_key_symbol::array_begin) {
				json_data.set_flag(type_flag_t::arr_t);
				parse_array(begin, end);
				return true;
			}
			else if (ch == parser::json_key_symbol::object_begin) {
				json_data.set_flag(type_flag_t::obj_t);
				parse_object(begin, end);
				return true;
			}
			else if (ch == parser::json_key_symbol::str) {
				get_next(begin, end);
				const char* b = *begin;
				parser::skip_str(begin, end);
				int len = *begin - b - 1;
				json_data.push_str_and_set(b, len);
				return true;
			}
			else {
				if (ch == 'f') {
					*begin += 5;
					json_data.push_num(0);
				}
				else if (ch == 't' || ch == 'n') {
					*begin += 4;
					json_data.push_num(0);
				}
				else {
					parser::unserialize(&json_data.push_num(), begin, end);
				}
				return true;
			}
			get_next(begin, end);
		}
		return false;
	}


	bool parse_object(const char** begin, const char* end) {
		size_t off = 0;
		// only the key_value should be parsed in {}
		while (char ch = parser::get_cur_and_next(begin, end)) {
			if (ch == parser::json_key_symbol::object_begin || ch == parser::json_key_symbol::next_key_value) {
				if (ch == parser::json_key_symbol::object_begin) {
					if (json_data.h->t != type_flag_t::obj_t) {
						json_data.push_head(type_flag_t::obj_t);
						json_data.set_child_or_length();
					}
				}
				if (**begin == json_key_symbol::object_end) {
					get_next(begin, end);
					json_data.h->cl = 0;
					return true;
				}
				off = json_data.data.size();
				parse_key_value(begin, end);
				json_data.set_next(off);
			}
			else if (ch == parser::json_key_symbol::object_end) {
				json_data.set_empty_next(off);
				return true;
			}
		}
		return false;
	}

	size_t parse_array(const char** begin, const char* end) {
		// skip the white space and control char
		skip_space(begin, end);

		//check the value type
		if (**begin != json_key_symbol::array_begin) {
			check_skip(begin, end);
			return false;
		}

		int i = 0;
		size_t off = 0;
		while (char ch = get_cur_and_next(begin, end)) {
			// '[' and ',' as the falg of value begin
			if (ch == json_key_symbol::array_begin || ch == json_key_symbol::next_key_value) {
				skip_space(begin, end);
				if (ch == parser::json_key_symbol::array_begin) {
					if (json_data.h->t != type_flag_t::arr_t) {
						json_data.push_head(type_flag_t::arr_t);
						json_data.set_child_or_length();
					}
				}
				if (**begin == json_key_symbol::array_end) {
					get_next(begin, end);
					json_data.h->cl = 0;
					return true;
				}
				off = json_data.data.size();
				json_data.push_head_and_set(type_flag_t::pre_t);
				unserialize(begin, end);
				json_data.set_next(off);
				i++;
			}
			else if (ch == json_key_symbol::array_end) {
				json_data.set_empty_next(off);
				return i;
			}
		}
		return i;
	}
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

		void set_int(int num) {
			*(double*)(get_val()) = num;
		}

		void set_string(const char* str, length_t len) {
			memcpy(get_val(), str, len);
		}

		int get_int() {
			if (t == type_flag_t::num_t)
				return *(double*)(get_val());
			return 0;
		}

		double get_double() {
			if (t == type_flag_t::num_t)
				return *(double*)(get_val());
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

		bool equal(int num) {
			if (get_int() == num)
				return true;
			return false;
		}
	};
#pragma pack(pop)

#define ckeck_v if (!v) return *this

	struct json {
		//const length_t head_size = sizeof(head_t);

		string data;
		head_t* h;

		json() {
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

		void set_empty_next(int off) {
			head_t *th = (head_t*)(data.data() + off);
			th->n = 0;
		}

		void set_child_or_length() {
			h->cl = data.size();
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
			if (h->t == type_flag_t::pre_t) {
				h->t = f;
				h->cl = data.size();
			}
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

		inline number_t& push_num() {
			h->t = type_flag_t::num_t;
			length_t head_off = (const char*)h - data.data();
			data.resize(data.size() + sizeof(number_t));
			h = (head_t*)(data.data() + head_off);
			//h->set_int(0);
			return *(number_t*)h->get_val();
		}

		inline void push_num(number_t num) {
			h->t = type_flag_t::num_t;
			length_t head_off = (const char*)h - data.data();
			data.resize(data.size() + sizeof(number_t));
			h = (head_t*)(data.data() + head_off);
			h->set_int(num);
		}

		json& operator [] (const char* key) {
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

		json& operator [] (int index) {
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

		void operator = (int num) {
			if (h) {
				if (h->t == type_flag_t::num_t)
					h->set_int(num);
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

		operator double()
		{
			return h->get_double();
		}

		operator int()
		{
			return h->get_int();
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

		bool operator == (int num) {
			if (h->t == type_flag_t::num_t)
				return h->equal(num);
			else
				return false;
		}

		bool operator == (const json &d) {
			return true;
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

		size_t size() {
			return h->cl;
		}

		void dump() {
			head_t *th = (head_t*)(data.data() + h->cl);
			const char* kkk = h->get_key();
			bool is_arr = false;
			bool is_obj = false;
			if (h->t == type_flag_t::obj_t) {
				//	cout << "\"" << th->get_key() << "\":";
				cout << "{";
				is_obj = true;
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
					}
					if (th->t == type_flag_t::num_t) {
						if (is_obj && th->kl)
							cout << "\"" << th->get_key() << "\":";
						cout << th->get_double() << ",";
					}
					else if (th->t == type_flag_t::str_t) {
						if (is_obj && th->kl)
							cout << "\"" << th->get_key() << "\":";
						cout << "\"" << th->get_string() << "\",";
					}
				}
				// return the end head
				if (!th->n) {
					cout << "\b";
					if (is_obj)
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
		json(const json &v) {}

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
	};
	json json_data;
	json& operator [] (const char* key) {
		json_data.h = (head_t*)json_data.data.data();
		return json_data[key];
		//json_data[key];
		//return *this;
	}

	json& operator [] (int index) {
		json_data.h = (head_t*)json_data.data.data();
		//return json_data[index];
		return json_data[index];
		//return *this;
	}

	int get_int(const char* key) {
		// json_data.h = (head_t*)json_data.data.data();
		return json_data[key];
	}

	string get_string(const char* key) {
		// json_data.h = (head_t*)json_data.data.data();
		return json_data[key];
	}

	int get_int(int index) {
		// json_data.h = (head_t*)json_data.data.data();
		return json_data[index];
	}

	string get_string(int index) {
		//json_data.h = (head_t*)json_data.data.data();
		return json_data[index];
	}

	void dump() {
		json_data.h = (head_t*)json_data.data.data();
		return json_data.dump();
	}

	void operator = (int num) {
		return json_data = num;
	}

	void operator = (const char* str) {
		return json_data = str;
	}

	operator int()
	{
		return json_data;
	}

	operator string() {
		return json_data;
	}

	operator const char*() {
		return json_data;
	}

	bool operator == (const char* str) {
		return json_data == str;
	}

	bool operator == (int num) {
		return json_data == num;
	}

	bool operator == (const json &d) {
		return true;
	}

	void erase() {
		return json_data.erase();
	}

	bool find(const char* key) {
		return json_data.find(key);
	}

	bool is_object() {
		return json_data.is_object();
	}

	bool is_array() {
		return json_data.is_array();
	}

	bool is_string() {
		return json_data.is_string();
	}

	bool is_number() {
		return json_data.is_number();
	}

	size_t size() {
		return json_data.size();
	}

public:
	// if *json end with '\0',don't need the size arg
	size_t unserialize(const char* json, size_t size = 0) {
		json_data.data.resize(0);
		json_data.h = (head_t*)json_data.data.data();
		const char* begin = json;
		const char* end = nullptr;
		if (size > 0)
			end = begin + size;
		unserialize(&begin, end);
		return begin - json;
	}
};