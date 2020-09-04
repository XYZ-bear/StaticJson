#pragma once

#include "static_json.h"
#include <vector>


class key_string {
public:
	const char* str;
	size_t len;
	key_string(const char* s) {
		str = s;
	}
};

namespace std {
	template<>
	struct hash<key_string> {
	public:
		size_t operator()(const key_string &p) const {
			unsigned long h = 0;
			const char *s = p.str;

			for (; *s; ++s) {
				if (*s == '"')
					break;
				else if (*s == '\\') {
					h = 5 * h + *s;
					if (*(++s))
						h = 5 * h + *s;
					else
						break;
				}
				else
					h = 5 * h + *s;
			}

			return size_t(h);
		}

	};

	template<>
	struct equal_to<key_string> {
	public:
		bool operator()(const key_string &p1, const key_string &p2) const
		{
			for (int i = 0;; i++) {
				if ((!p1.str[i] || p1.str[i] == '"') || (!p2.str[i] || p2.str[i] == '"')) {
					break;
				}
				if (p1.str[i] != p2.str[i])
					return false;
				else {
					if (p1.str[i] == '\\') {
						i++;
						if (p1.str[i] != p2.str[i])
							return false;
					}
				}

			}
			return true;
		}

	};
}


//class dynamic_json:public parser {
//public:
//
//
//	struct value_t {
//		value_t() {
//			memset(data, 0, sizeof(data_t) + 1);
//		}
//		union data_t
//		{
//			double d;
//			string *s;
//			vector<value_t>* v;
//			map<string, value_t>* o;
//		};
//		char data[sizeof(data_t) + 1];
//
//		data_t &get_data() {
//			return *(data_t*)data;
//		}
//		char& get_type() {
//			return data[sizeof(data_t)];
//		}
//		value_t& add_o(string &key) {
//			data[sizeof(data_t)] = 4;
//			auto &d = get_data();
//			if (!d.o)
//				d.o = new map<string, value_t>;
//			return (*d.o)[key];
//		}
//		vector<value_t>& add_v() {
//			data[sizeof(data_t)] = 3;
//			auto &d = get_data();
//			if (!d.v)
//				d.v = new vector<value_t>;
//			return (*d.v);
//		}
//		double &add_d() {
//			data[sizeof(data_t)] = 1;
//			return get_data().d;
//		}
//		string &add_s() {
//			data[sizeof(data_t)] = 2;
//			auto &d = get_data();
//			if (!d.v)
//				d.s = new string;
//			return *d.s;
//		}
//
//		void print() {
//			auto &d = get_data();
//			auto t = get_type();
//			if (t == 1) {
//				cout << d.d << ",";
//			}
//			else if (t == 2) {
//				cout << "\"" << *d.s << "\",";
//			}
//			else if (t == 3) {
//				cout << "[";
//				for (auto &v : *d.v) {
//					v.print();
//					cout << ",";
//				}
//				cout << "]";
//			}
//			else if (t == 4) {
//				cout << "{";
//				for (auto &i : *d.o) {
//					cout << "\"" << i.first << "\":";
//					i.second.print();
//				}
//			}
//		}
//
//		value_t(const value_t& c)
//		{
//
//		}
//
//		value_t(value_t& c)
//		{
//			get_data() = c.get_data();
//			get_type() = c.get_type();
//			c.get_type() = 0;
//			auto t = get_type();
//		}
//
//		value_t & operator [](const char* k) {
//			if (get_type() == 4) {
//				auto &d = get_data();
//				return (*d.o)[k];
//			}
//		}
//
//		double get_double() {
//			if (get_type() == 1) {
//				auto &d = get_data();
//				return d.d;
//			}
//			return 0;
//		}
//
//		~value_t() {
//			auto& d = get_data();
//			char t = get_type();
//			if (t) {
//				if (t == 2)
//					delete d.s;
//				else if (t == 3)
//					delete d.v;
//				else if (t == 4)
//					delete d.o;
//			}
//		}
//
//		size_t size() {
//			int i = sizeof(*this);
//			auto& d = get_data();
//			char t = get_type();
//			if (t) {
//				if (t == 2) {
//					i += sizeof(*d.s);
//					i += d.s->capacity();
//				}
//				else if (t == 3) {
//					i += sizeof(*d.v);
//					for (auto &v : *d.v) {
//						i += v.size();
//					}
//					i += (d.v->capacity() - d.v->size()) * sizeof(value_t);
//				}
//				else if (t == 4) {
//					i += sizeof(*d.o);
//					for (auto &v : *d.o) {
//						//i += sizeof(v);
//						i += v.first.capacity();
//						i += v.second.size();
//					}
//				}
//			}
//			return i;
//		}
//	};
//
//	value_t value;
//
//	dynamic_json() {
//	}
//	~dynamic_json() {
//	}
//	bool parse_key_value(value_t *val,const char** begin, const char* end) {
//		parser::skip_space(begin, end);
//		if (parser::get_cur_and_next(begin, end) == parser::json_key_symbol::str) {
//			string key;
//			parser::parse_str(key, begin, end);
//			parser::skip_space(begin, end);
//			if (parser::get_cur_and_next(begin, end) == parser::json_key_symbol::key_value_separator) {
//				parser::skip_space(begin, end);
//				auto & v = val->add_o(key);
//				return unserialize(&v, begin, end);
//			}
//			else
//				return false;
//		}
//		return false;
//	}
//
//	bool unserialize(value_t *val, const char** begin, const char* end) {
//		if (char ch = **begin) {
//			if (ch == parser::json_key_symbol::array_begin) {
//				return unserialize(&val->add_v(), begin, end);
//			}
//			else if (ch == parser::json_key_symbol::object_begin) {
//				return parse_object(val, begin, end);
//			}
//			else if (ch == parser::json_key_symbol::str) {
//				get_next(begin, end);
//				return parser::parse_str(val->add_s(), begin, end);
//			}
//			else {
//				if (ch == 'f') {
//					*begin += 5;
//				}
//				else if (ch == 't' || ch == 'n') {
//					*begin += 4;
//				}
//				else {
//					parser::unserialize(&val->add_d(), begin, end);
//				}
//				return true;
//			}
//			get_next(begin, end);
//		}
//		return false;
//	}
//
//
//	bool parse_object(value_t *val, const char** begin, const char* end) {
//		// only the key_value should be parsed in {}
//		while (char ch = parser::get_cur_and_next(begin, end)) {
//			if (ch == parser::json_key_symbol::object_begin || ch == parser::json_key_symbol::next_key_value) {
//				parse_key_value(val, begin, end);
//			}
//			else if (ch == parser::json_key_symbol::object_end) {
//				return true;
//			}
//		}
//		return false;
//	}
//
//	bool unserialize(vector<value_t> *data, const char** begin, const char* end) {
//		// skip the white space and control char
//		skip_space(begin, end);
//
//		//check the value type
//		if (**begin != json_key_symbol::array_begin) {
//			check_skip(begin, end);
//			return false;
//		}
//
//		data->reserve(2);
//		while (char ch = get_cur_and_next(begin, end)) {
//			// '[' and ',' as the falg of value begin
//			if (ch == json_key_symbol::array_begin || ch == json_key_symbol::next_key_value) {
//				skip_space(begin, end);
//
//				if (**begin == json_key_symbol::array_end)
//					return true;
//
//				size_t index = data->size();
//				data->resize(index + 1);
//				unserialize(&((*data)[index]), begin, end);
//
//			}
//			else if (ch == json_key_symbol::array_end) {
//				return true;
//			}
//		}
//		return false;
//	}
//public:
//	// if *json end with '\0',don't need the size arg
//	size_t unserialize(const char* json, size_t size = 0) {
//		const char* begin = json;
//		const char* end = nullptr;
//		if (size > 0)
//			end = begin + size;
//		parse_object(&value,&begin, end);
//
//		return begin - json;
//	}
//};

//class dynamic_json :public parser {
//public:
//
//
//	struct value_t {
//		value_t() {
//			memset(data, 0, sizeof(data_t) + 1);
//		}
//
//		struct member {
//			const char* key;
//			size_t key_len;
//			value_t *v;
//		};
//		union data_t
//		{
//			double d;
//			string *s;
//			vector<value_t>* v;
//			vector<member>* o;
//		};
//		char data[sizeof(data_t) + 1];
//
//		data_t &get_data() {
//			return *(data_t*)data;
//		}
//		char& get_type() {
//			return data[sizeof(data_t)];
//		}
//		value_t& add_o(const char* c,size_t len) {
//			data[sizeof(data_t)] = 4;
//			auto &d = get_data();
//			if (!d.o)
//				d.o = new vector<member>;
//			size_t index = d.o->size();
//			d.o->resize(index + 1);
//			member &v = (*d.o)[index];
//			v.key = new char(len);
//			v.key_len = len;
//			v.v = new value_t;
//			return *v.v;
//		}
//		vector<value_t>& add_v() {
//			data[sizeof(data_t)] = 3;
//			auto &d = get_data();
//			if (!d.v)
//				d.v = new vector<value_t>;
//			return (*d.v);
//		}
//		double &add_d() {
//			data[sizeof(data_t)] = 1;
//			return get_data().d;
//		}
//		string &add_s() {
//			data[sizeof(data_t)] = 2;
//			auto &d = get_data();
//			if (!d.v)
//				d.s = new string;
//			return *d.s;
//		}
//
//		void print() {
//			auto &d = get_data();
//			auto t = get_type();
//			if (t == 1) {
//				cout << d.d << ",";
//			}
//			else if (t == 2) {
//				cout << "\"" << *d.s << "\",";
//			}
//			else if (t == 3) {
//				cout << "[";
//				for (auto &v : *d.v) {
//					v.print();
//					cout << ",";
//				}
//				cout << "]";
//			}
//			else if (t == 4) {
//				cout << "{";
//				for (auto &i : *d.o) {
//					//cout << "\"" << i.first << "\":";
//					i.v->print();
//				}
//			}
//		}
//
//		value_t(const value_t& c)
//		{
//
//		}
//
//		value_t(value_t& c)
//		{
//			get_data() = c.get_data();
//			get_type() = c.get_type();
//			c.get_type() = 0;
//			auto t = get_type();
//		}
//
//		//value_t & operator [](const char* k) {
//		//	if (get_type() == 4) {
//		//		auto &d = get_data();
//		//		return (*d.o)[k];
//		//	}
//		//}
//
//		double get_double() {
//			if (get_type() == 1) {
//				auto &d = get_data();
//				return d.d;
//			}
//			return 0;
//		}
//
//		~value_t() {
//			auto& d = get_data();
//			char t = get_type();
//			if (t) {
//				if (t == 2)
//					delete d.s;
//				else if (t == 3)
//					delete d.v;
//				else if (t == 4) {
//					for (auto &i : *d.o) {
//						delete[] i.key;
//						delete i.v;
//					}
//					delete d.o;
//				}
//			}
//		}
//
//		size_t size() {
//			int i = sizeof(*this);
//			auto& d = get_data();
//			char t = get_type();
//			if (t) {
//				if (t == 2) {
//					i += sizeof(*d.s);
//					i += d.s->size();
//				}
//				else if (t == 3) {
//					i += sizeof(*d.v);
//					for (auto &v : *d.v) {
//						i += v.size();
//					}
//					i += (d.v->capacity() - d.v->size()) * sizeof(value_t);
//				}
//				else if (t == 4) {
//					i += sizeof(*d.o);
//					for (auto &v : *d.o) {
//						i += sizeof(v);
//						i += v.key_len;
//						i += v.v->size();
//					}
//					i += (d.o->capacity() - d.o->size()) * sizeof(member);
//				}
//			}
//			return i;
//		}
//	};
//
//	value_t value;
//
//	dynamic_json() {
//	}
//	~dynamic_json() {
//	}
//	bool parse_key_value(value_t *val, const char** begin, const char* end) {
//		parser::skip_space(begin, end);
//		if (parser::get_cur_and_next(begin, end) == parser::json_key_symbol::str) {
//			const char* b = *begin;
//			parser::skip_str(begin, end);
//			int len = *begin - b - 1;
//			parser::skip_space(begin, end);
//			if (parser::get_cur_and_next(begin, end) == parser::json_key_symbol::key_value_separator) {
//				parser::skip_space(begin, end);
//				auto & v = val->add_o(b, len);
//				return unserialize(&v, begin, end);
//			}
//			else
//				return false;
//		}
//		return false;
//	}
//
//	bool unserialize(value_t *val, const char** begin, const char* end) {
//		if (char ch = **begin) {
//			if (ch == parser::json_key_symbol::array_begin) {
//				return unserialize(&val->add_v(), begin, end);
//			}
//			else if (ch == parser::json_key_symbol::object_begin) {
//				return parse_object(val, begin, end);
//			}
//			else if (ch == parser::json_key_symbol::str) {
//				get_next(begin, end);
//				return parser::parse_str(val->add_s(), begin, end);
//			}
//			else {
//				if (ch == 'f') {
//					*begin += 5;
//				}
//				else if (ch == 't' || ch == 'n') {
//					*begin += 4;
//				}
//				else {
//					parser::unserialize(&val->add_d(), begin, end);
//				}
//				return true;
//			}
//			get_next(begin, end);
//		}
//		return false;
//	}
//
//
//	bool parse_object(value_t *val, const char** begin, const char* end) {
//		// only the key_value should be parsed in {}
//		while (char ch = parser::get_cur_and_next(begin, end)) {
//			if (ch == parser::json_key_symbol::object_begin || ch == parser::json_key_symbol::next_key_value) {
//				parse_key_value(val, begin, end);
//			}
//			else if (ch == parser::json_key_symbol::object_end) {
//				return true;
//			}
//		}
//		return false;
//	}
//
//	bool unserialize(vector<value_t> *data, const char** begin, const char* end) {
//		// skip the white space and control char
//		skip_space(begin, end);
//
//		//check the value type
//		if (**begin != json_key_symbol::array_begin) {
//			check_skip(begin, end);
//			return false;
//		}
//
//		data->reserve(2);
//		while (char ch = get_cur_and_next(begin, end)) {
//			// '[' and ',' as the falg of value begin
//			if (ch == json_key_symbol::array_begin || ch == json_key_symbol::next_key_value) {
//				skip_space(begin, end);
//
//				if (**begin == json_key_symbol::array_end)
//					return true;
//
//				size_t index = data->size();
//				data->resize(index + 1);
//				unserialize(&((*data)[index]), begin, end);
//
//			}
//			else if (ch == json_key_symbol::array_end) {
//				return true;
//			}
//		}
//		return false;
//	}
//public:
//	// if *json end with '\0',don't need the size arg
//	size_t unserialize(const char* json, size_t size = 0) {
//		const char* begin = json;
//		const char* end = nullptr;
//		if (size > 0)
//			end = begin + size;
//		parse_object(&value, &begin, end);
//
//		return begin - json;
//	}
//};

//class dynamic_json :public parser {
//public:
//	typedef char flag_t;
//	typedef uint32_t next_t;
//	typedef uint32_t length_t;
//	typedef double number_t;
//	enum type_flag_t
//	{
//		pre_t = 1,
//		num_t = 2,
//		nul_t = 3,
//		boo_t = 4,
//		str_t = 5,
//		arr_t = 6,
//		obj_t = 7,
//		del_t = 8
//	};
//	dynamic_json() {
//		//json_data.reserve(1024);
//	}
//	~dynamic_json() {
//	}
//
//	bool parse_key_value(const char** begin, const char* end) {
//		parser::skip_space(begin, end);
//		if (parser::get_cur_and_next(begin, end) == parser::json_key_symbol::str) {
//			const char* b = *begin;
//			parser::skip_str(begin, end);
//			int len = *begin - b - 1;
//			parser::skip_space(begin, end);
//			if (parser::get_cur_and_next(begin, end) == parser::json_key_symbol::key_value_separator) {
//				json_data.push_key(b, len);
//				return unserialize(begin, end);
//			}
//			else
//				return false;
//		}
//		return false;
//	}
//
//	bool unserialize(const char** begin, const char* end) {
//		skip_space(begin, end);
//		if (char ch = **begin) {
//			if (ch == parser::json_key_symbol::array_begin) {
//				length_t array_size_off = json_data.push_head(type_flag_t::arr_t);
//				length_t array_bit_off = json_data.push_length_placeholder();
//				length_t pre = json_data.data.size();
//				json_data.set_length(array_size_off, parse_array(begin, end));
//				json_data.set_length(array_bit_off, json_data.data.size() - pre);
//				return true;
//			}
//			else if (ch == parser::json_key_symbol::object_begin) {
//				length_t off = json_data.push_head(type_flag_t::obj_t);
//				length_t pre = json_data.data.size();
//				parse_object(begin, end);
//				json_data.set_length(off, json_data.data.size() - pre);
//				return true;
//			}
//			else if (ch == parser::json_key_symbol::str) {
//				get_next(begin, end);
//				const char* b = *begin;
//				parser::skip_str(begin, end);
//				int len = *begin - b - 1;
//				json_data.push_str(b, len);
//				return true;
//			}
//			else {
//				if (ch == 'f') {
//					*begin += 5;
//					json_data.push_num(0);
//				}
//				else if (ch == 't' || ch == 'n') {
//					*begin += 4;
//					json_data.push_num(0);
//				}
//				else {
//					parser::unserialize(&json_data.push_num(), begin, end);
//				}
//				return true;
//			}
//			get_next(begin, end);
//		}
//		return false;
//	}
//
//
//	bool parse_object(const char** begin, const char* end) {
//		// only the key_value should be parsed in {}
//		while (char ch = parser::get_cur_and_next(begin, end)) {
//			if (ch == parser::json_key_symbol::object_begin || ch == parser::json_key_symbol::next_key_value) {
//				parse_key_value(begin, end);
//			}
//			else if (ch == parser::json_key_symbol::object_end) {
//				return true;
//			}
//		}
//		return false;
//	}
//
//	size_t parse_array(const char** begin, const char* end) {
//		// skip the white space and control char
//		skip_space(begin, end);
//
//		//check the value type
//		if (**begin != json_key_symbol::array_begin) {
//			check_skip(begin, end);
//			return false;
//		}
//
//		int i = 0;
//		while (char ch = get_cur_and_next(begin, end)) {
//			// '[' and ',' as the falg of value begin
//			if (ch == json_key_symbol::array_begin || ch == json_key_symbol::next_key_value) {
//				skip_space(begin, end);
//
//				if (**begin == json_key_symbol::array_end)
//					return true;
//
//				unserialize(begin, end);
//				i++;
//			}
//			else if (ch == json_key_symbol::array_end) {
//				return i;
//			}
//		}
//		return i;
//	}
//#pragma pack(push,1)
//	struct values_t
//	{
//		char t;
//		int n;
//		int l;
//	private:
//		values_t() {
//			t = 0;
//			n = 0;
//			l = 0;
//		}
//		values_t(const values_t &v) {}
//	};
//#pragma pack(pop)
//
//#define ckeck_v if (!v) return *this
//	struct json {
//		string data;
//		values_t* v;
//
//		json() {
//		}
//
//		void push_key(const char* k, length_t len) {
//			data.append((const char*)&(++len), sizeof(int));
//			data.append(k, len);
//			*(data.end() - 1) = '\0';
//		}
//
//		length_t push_head(flag_t f) {
//			data.append(sizeof(flag_t), f);
//			data.append(sizeof(next_t), '\0');
//			length_t off = data.size();
//			data.append(sizeof(length_t), '\0');
//			return off;
//		}
//
//		length_t push_length_placeholder() {
//			length_t off = data.size();
//			data.append(sizeof(length_t), '\0');
//			return off;
//		}
//
//		void set_length(length_t off, length_t len) {
//			*(length_t*)(data.data() + off) = len;
//		}
//
//		void push_str(const char* k, length_t len) {
//			data.append(sizeof(flag_t), type_flag_t::str_t);
//			data.append(sizeof(next_t), '\0');
//			data.append((const char*)&(++len), sizeof(length_t));
//			data.append(k, len);
//			*(data.end() - 1) = '\0';
//		}
//
//		number_t& push_num() {
//			data.append(sizeof(flag_t), type_flag_t::num_t);
//			size_t size = data.size();
//			data.append(sizeof(number_t), '0');
//			return *(number_t*)(data.data() + size);
//		}
//
//		void push_num(number_t d) {
//			data.append(sizeof(flag_t), type_flag_t::num_t);
//			data.append((const char*)&d, sizeof(number_t));
//		}
//
//		const char* data_end() {
//			return data.data() + data.size();
//		}
//
//		json& operator [] (const char* key) {
//			ckeck_v;
//			v = get_key_val(key);
//			return *this;
//		}
//
//		json& operator [] (int i) {
//			ckeck_v;
//			v = get_array_val(i);
//			return *this;
//		}
//
//		int get_int(int index) {
//			return get_int(get_array_val(index));
//		}
//
//		int get_int(const char* key) {
//			return get_int(get_key_val(key));
//		}
//
//		const char* get_string(int index) {
//			return get_string(get_array_val(index));
//		}
//
//		const char* get_string(const char* key) {
//			return get_string(get_key_val(key));
//		}
//
//		int get_size() {
//			if (v)
//				return v->l;
//			return 0;
//		}
//
//		void operator = (const char* str) {
//			if (v) {
//				if (v->t == type_flag_t::str_t && str) {
//					int len = strlen(str) + 1;
//					if (len < v->l) {
//						memcpy(((char*)v + sizeof(values_t)), str, len);
//						v->l = len;
//					}
//					else {
//						v->n = data.size();
//						push_str(str, len);
//					}
//				}
//			}
//		}
//
//		void operator = (int num) {
//			if (v) {
//				if (v->t == type_flag_t::num_t)
//					*(number_t*)((char*)v + sizeof(flag_t)) = num;
//			}
//		}
//
//		private:
//			json(const json &v) {}
//			int get_int(const values_t *val) {
//				if (val && val->t == type_flag_t::num_t) {
//					return *(double*)((const char*)val + sizeof(flag_t));
//				}
//				return 0;
//			}
//
//			const char* get_string(const values_t *val) {
//				if (val && val->t == type_flag_t::str_t) {
//					return ((const char*)val + sizeof(values_t));
//				}
//				return "";
//			}
//
//			values_t* to_link_next(values_t *val) {
//				while (val && val->n) {
//					val = (values_t*)(data.data() + val->n);
//				}
//				return val;
//			}
//
//			values_t* get_array_val(size_t i) {
//				if (v && v->t != type_flag_t::arr_t || i<0 || i>v->l) {
//					return nullptr;
//				}
//
//				const char* begin = (const char*)v + sizeof(values_t);
//				length_t l = *(length_t*)begin; begin += sizeof(length_t);
//				const char* end = begin + l;
//				int index = 0;
//				while (begin < end) {
//					values_t *v = (values_t*)begin;
//
//					if (i == index) {
//						return v;
//					}
//
//					if (v->t == type_flag_t::num_t) {
//						begin += sizeof(flag_t) + sizeof(double);
//					}
//					else {
//						begin += sizeof(values_t);
//						begin += v->l;
//					}
//					index++;
//				}
//				return nullptr;
//			}
//
//			values_t* get_key_val(const char* key) {
//				if (v->t != type_flag_t::obj_t)
//					return nullptr;
//				const char* begin = (const char*)v + sizeof(values_t);
//				const char* end = begin + v->l;
//				while (begin < end) {
//					int kl = *(int*)begin; begin += sizeof(int);
//					const char* k = begin; begin += kl;
//
//					values_t *v = (values_t*)begin;
//					if (!strcmp(key, k)) {
//						v = to_link_next(v);
//						return v;
//					}
//
//					if (v->t == type_flag_t::num_t) {
//						begin += sizeof(flag_t) + sizeof(double);
//					}
//					else {
//						begin += sizeof(values_t);
//						begin += v->l;
//					}
//				}
//				v = nullptr;
//				return nullptr;
//			}
//
//	};
//	json json_data;
//	json& operator [] (const char* key) {
//		json_data.v = (values_t*)json_data.data.data();
//		return json_data[key];
//	}
//
//	int get_int(int index) {
//		json_data.v = (values_t*)json_data.data.data();
//		return json_data.get_int(index);
//	}
//
//	int get_int(const char* key) {
//		json_data.v = (values_t*)json_data.data.data();
//		return json_data.get_int(key);
//	}
//
//	const char* get_string(int index) {
//		json_data.v = (values_t*)json_data.data.data();
//		return json_data.get_string(index);
//	}
//
//	const char* get_string(const char* key) {
//		json_data.v = (values_t*)json_data.data.data();
//		return json_data.get_string(key);
//	}
//public:
//	// if *json end with '\0',don't need the size arg
//	size_t unserialize(const char* json, size_t size = 0) {
//		json_data.data.resize(0);
//		const char* begin = json;
//		const char* end = nullptr;
//		if (size > 0)
//			end = begin + size;
//		unserialize(&begin, end);
//		return begin - json;
//	}
//};

//class dynamic_json :public parser {
//public:
//	typedef char flag_t;
//	typedef uint32_t next_t;
//	typedef uint32_t length_t;
//	typedef double number_t;
//	enum type_flag_t
//	{
//		emp_t = 0,
//		pre_t = 1,
//		num_t = 2,
//		nul_t = 3,
//		boo_t = 4,
//		str_t = 5,
//		arr_t = 6,
//		obj_t = 7,
//		del_t = 8
//	};
//	dynamic_json() {
//		//json_data.reserve(1024);
//	}
//	~dynamic_json() {
//	}
//
//	bool parse_key_value(const char** begin, const char* end) {
//		parser::skip_space(begin, end);
//		if (parser::get_cur_and_next(begin, end) == parser::json_key_symbol::str) {
//			const char* b = *begin;
//			parser::skip_str(begin, end);
//			int len = *begin - b - 1;
//			parser::skip_space(begin, end);
//			if (parser::get_cur_and_next(begin, end) == parser::json_key_symbol::key_value_separator) {
//				json_data.push_key(b, len);
//				return unserialize(begin, end);
//			}
//			else
//				return false;
//		}
//		return false;
//	}
//
//	bool unserialize(const char** begin, const char* end) {
//		skip_space(begin, end);
//		if (char ch = **begin) {
//			if (ch == parser::json_key_symbol::array_begin) {
//				length_t array_size_off = json_data.push_head(type_flag_t::arr_t);
//				length_t array_bit_off = json_data.push_length_placeholder();
//				length_t pre = json_data.data.size();
//				json_data.set_length(array_size_off, parse_array(begin, end));
//				json_data.set_length(array_bit_off, json_data.data.size() - pre);
//				return true;
//			}
//			else if (ch == parser::json_key_symbol::object_begin) {
//				length_t off = json_data.push_head(type_flag_t::obj_t);
//				length_t pre = json_data.data.size();
//				parse_object(begin, end);
//				json_data.set_length(off, json_data.data.size() - pre);
//				return true;
//			}
//			else if (ch == parser::json_key_symbol::str) {
//				get_next(begin, end);
//				const char* b = *begin;
//				parser::skip_str(begin, end);
//				int len = *begin - b - 1;
//				json_data.push_str(b, len);
//				return true;
//			}
//			else {
//				if (ch == 'f') {
//					*begin += 5;
//					json_data.push_num(0);
//				}
//				else if (ch == 't' || ch == 'n') {
//					*begin += 4;
//					json_data.push_num(0);
//				}
//				else {
//					parser::unserialize(&json_data.push_num(), begin, end);
//				}
//				return true;
//			}
//			get_next(begin, end);
//		}
//		return false;
//	}
//
//
//	bool parse_object(const char** begin, const char* end) {
//		// only the key_value should be parsed in {}
//		while (char ch = parser::get_cur_and_next(begin, end)) {
//			if (ch == parser::json_key_symbol::object_begin || ch == parser::json_key_symbol::next_key_value) {
//				parse_key_value(begin, end);
//			}
//			else if (ch == parser::json_key_symbol::object_end) {
//				return true;
//			}
//		}
//		return false;
//	}
//
//	size_t parse_array(const char** begin, const char* end) {
//		// skip the white space and control char
//		skip_space(begin, end);
//
//		//check the value type
//		if (**begin != json_key_symbol::array_begin) {
//			check_skip(begin, end);
//			return false;
//		}
//
//		int i = 0;
//		while (char ch = get_cur_and_next(begin, end)) {
//			// '[' and ',' as the falg of value begin
//			if (ch == json_key_symbol::array_begin || ch == json_key_symbol::next_key_value) {
//				skip_space(begin, end);
//
//				if (**begin == json_key_symbol::array_end)
//					return true;
//
//				unserialize(begin, end);
//				i++;
//			}
//			else if (ch == json_key_symbol::array_end) {
//				return i;
//			}
//		}
//		return i;
//	}
//#pragma pack(push,1)
//	struct values_t
//	{
//		char t;
//		int n;
//		int l;
//	private:
//		values_t() {
//			t = 0;
//			n = 0;
//			l = 0;
//		}
//		values_t(const values_t &v) {}
//	};
//#pragma pack(pop)
//
//#define ckeck_v if (!v) return *this
//	struct json {
//		string data;
//		values_t* v;
//
//		json() {
//		}
//
//		void push_key(const char* k, length_t len) {
//			data.append((const char*)&(++len), sizeof(int));
//			data.append(k, len);
//			*(data.end() - 1) = '\0';
//		}
//
//		length_t push_head(flag_t f) {
//			data.append(sizeof(flag_t), f);
//			data.append(sizeof(next_t), '\0');
//			length_t off = data.size();
//			data.append(sizeof(length_t), '\0');
//			return off;
//		}
//
//		length_t push_length_placeholder() {
//			length_t off = data.size();
//			data.append(sizeof(length_t), '\0');
//			return off;
//		}
//
//		void set_length(length_t off, length_t len) {
//			*(length_t*)(data.data() + off) = len;
//		}
//
//		void push_str(const char* k, length_t len) {
//			data.append(sizeof(flag_t), type_flag_t::str_t);
//			data.append(sizeof(next_t), '\0');
//			data.append((const char*)&(++len), sizeof(length_t));
//			data.append(k, len);
//			*(data.end() - 1) = '\0';
//		}
//
//		number_t& push_num() {
//			data.append(sizeof(flag_t), type_flag_t::num_t);
//			size_t size = data.size();
//			data.append(sizeof(number_t), '0');
//			return *(number_t*)(data.data() + size);
//		}
//
//		void push_num(number_t d) {
//			data.append(sizeof(flag_t), type_flag_t::num_t);
//			data.append((const char*)&d, sizeof(number_t));
//		}
//
//		const char* data_end() {
//			return data.data() + data.size();
//		}
//
//		json& operator [] (const char* key) {
//			ckeck_v;
//			if (data.size() == 0) {
//				length_t next = push_head(type_flag_t::obj_t);
//				set_length(next, data.size());
//				push_key(key, strlen(key));
//				int off = data.size();
//				push_head(type_flag_t::pre_t);
//				v = (values_t*)(data.data() + off);
//			}
//			else {
//				if (v->t == type_flag_t::pre_t) {
//					v->t = type_flag_t::obj_t;
//					v->l = data.size();
//					push_key(key, strlen(key));
//					int off = data.size();
//					push_head(type_flag_t::pre_t);
//					v = (values_t*)(data.data() + off);
//				}
//				else {
//					values_t *tv = get_key_val(key);
//					if (tv && v != tv) {
//						tv->n = data.size();
//						push_key(key, strlen(key));
//						int off = data.size();
//						push_head(type_flag_t::pre_t);
//						tv = (values_t*)(data.data() + off);
//					}
//					v = tv;
//				}
//			}
//			return *this;
//		}
//
//		json& operator [] (int i) {
//			ckeck_v;
//			v = get_array_val(i);
//			return *this;
//		}
//
//		int get_int(int index) {
//			return get_int(get_array_val(index));
//		}
//
//		int get_int(const char* key) {
//			return get_int(get_key_val(key));
//		}
//
//		const char* get_string(int index) {
//			return get_string(get_array_val(index));
//		}
//
//		const char* get_string(const char* key) {
//			return get_string(get_key_val(key));
//		}
//
//		int get_size() {
//			if (v)
//				return v->l;
//			return 0;
//		}
//
//		void operator = (const char* str) {
//			if (v) {
//				if (str) {
//					int len = strlen(str) + 1;
//					if (v->t == type_flag_t::str_t) {
//						if (len < v->l) {
//							memcpy(((char*)v + sizeof(values_t)), str, len);
//							v->l = len;
//						}
//						else {
//							v->t = type_flag_t::del_t;
//							v->n = data.size();
//						}
//					}
//					else if (v->t == type_flag_t::pre_t) {
//						v->t = type_flag_t::str_t;
//						v->l = len;
//						data.append(str, len);
//						*(data.end() - 1) = '\0';
//					}
//				}
//			}
//		}
//
//		void operator = (int num) {
//			if (v) {
//				if (v->t == type_flag_t::num_t)
//					*(int*)((char*)v + sizeof(flag_t) + sizeof(next_t)) = num;
//				else if (v->t == type_flag_t::pre_t) {
//					v->t = type_flag_t::num_t;
//					data.resize(data.size() - sizeof(length_t));
//					data.append((const char*)&num, sizeof(number_t));
//				}
//			}
//		}
//
//		bool erase() {
//			if (v) {
//				v->t = type_flag_t::del_t;
//				return true;
//			}
//			return false;
//		}
//
//	private:
//		json(const json &v) {}
//		int get_int(const values_t *val) {
//			if (val && val->t == type_flag_t::num_t) {
//				return *(int*)((const char*)val + sizeof(flag_t) + sizeof(next_t));
//			}
//			return 0;
//		}
//
//		const char* get_string(const values_t *val) {
//			if (val && val->t == type_flag_t::str_t) {
//				return ((const char*)val + sizeof(values_t));
//			}
//			return "";
//		}
//
//		values_t* to_link_next(values_t *val) {
//			while (val && val->n) {
//				val = (values_t*)(data.data() + val->n);
//			}
//			return val;
//		}
//
//		values_t* get_array_val(size_t i) {
//			if (v && v->t != type_flag_t::arr_t || i<0 || i>v->l) {
//				return nullptr;
//			}
//
//			const char* begin = (const char*)v + sizeof(values_t);
//			length_t l = *(length_t*)begin; begin += sizeof(length_t);
//			const char* end = begin + l;
//			int index = 0;
//			while (begin < end) {
//				values_t *v = (values_t*)begin;
//
//				if (i == index) {
//					return v;
//				}
//
//				if (v->t == type_flag_t::num_t) {
//					begin += sizeof(flag_t) + sizeof(double);
//				}
//				else {
//					begin += sizeof(values_t);
//					begin += v->l;
//				}
//				index++;
//			}
//			return nullptr;
//		}
//
//		values_t* get_key_val(const char* key) {
//			if (v->t != type_flag_t::obj_t)
//				return nullptr;
//			const char* begin = data.data() + v->l;
//			while (1) {
//				int kl = *(int*)begin; begin += sizeof(int);
//				const char* k = begin; begin += kl;
//
//				values_t *tv = (values_t*)begin;
//				if (!strcmp(key, k)) {
//					v = tv;
//					return tv;
//				}
//				if (tv->n == 0)
//					return tv;
//				begin = data.data() + tv->n;
//			}
//			return nullptr;
//		}
//
//	};
//	json json_data;
//	json& operator [] (const char* key) {
//		json_data.v = (values_t*)json_data.data.data();
//		return json_data[key];
//	}
//
//	int get_int(int index) {
//		json_data.v = (values_t*)json_data.data.data();
//		return json_data.get_int(index);
//	}
//
//	int get_int(const char* key) {
//		json_data.v = (values_t*)json_data.data.data();
//		return json_data.get_int(key);
//	}
//
//	const char* get_string(int index) {
//		json_data.v = (values_t*)json_data.data.data();
//		return json_data.get_string(index);
//	}
//
//	const char* get_string(const char* key) {
//		json_data.v = (values_t*)json_data.data.data();
//		return json_data.get_string(key);
//	}
//public:
//	// if *json end with '\0',don't need the size arg
//	size_t unserialize(const char* json, size_t size = 0) {
//		json_data.data.resize(0);
//		const char* begin = json;
//		const char* end = nullptr;
//		if (size > 0)
//			end = begin + size;
//		unserialize(&begin, end);
//		return begin - json;
//	}
//};

class dynamic_json :public parser {
public:
	typedef char flag_t;
	typedef uint32_t next_t;
	typedef uint32_t length_t;
	typedef double number_t;
	typedef uint32_t key_t;
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

	//bool parse_key_value(const char** begin, const char* end) {
	//	parser::skip_space(begin, end);
	//	if (parser::get_cur_and_next(begin, end) == parser::json_key_symbol::str) {
	//		const char* b = *begin;
	//		parser::skip_str(begin, end);
	//		int len = *begin - b - 1;
	//		parser::skip_space(begin, end);
	//		if (parser::get_cur_and_next(begin, end) == parser::json_key_symbol::key_value_separator) {
	//			json_data.push_key(b, len);
	//			return unserialize(begin, end);
	//		}
	//		else
	//			return false;
	//	}
	//	return false;
	//}

	//bool unserialize(const char** begin, const char* end) {
	//	skip_space(begin, end);
	//	if (char ch = **begin) {
	//		if (ch == parser::json_key_symbol::array_begin) {
	//			length_t array_size_off = json_data.push_head(type_flag_t::arr_t);
	//			length_t array_bit_off = json_data.push_length_placeholder();
	//			length_t pre = json_data.data.size();
	//			json_data.set_length(array_size_off, parse_array(begin, end));
	//			json_data.set_length(array_bit_off, json_data.data.size() - pre);
	//			return true;
	//		}
	//		else if (ch == parser::json_key_symbol::object_begin) {
	//			length_t off = json_data.push_head(type_flag_t::obj_t);
	//			length_t pre = json_data.data.size();
	//			parse_object(begin, end);
	//			json_data.set_length(off, json_data.data.size() - pre);
	//			return true;
	//		}
	//		else if (ch == parser::json_key_symbol::str) {
	//			get_next(begin, end);
	//			const char* b = *begin;
	//			parser::skip_str(begin, end);
	//			int len = *begin - b - 1;
	//			json_data.push_str(b, len);
	//			return true;
	//		}
	//		else {
	//			if (ch == 'f') {
	//				*begin += 5;
	//				json_data.push_num(0);
	//			}
	//			else if (ch == 't' || ch == 'n') {
	//				*begin += 4;
	//				json_data.push_num(0);
	//			}
	//			else {
	//				parser::unserialize(&json_data.push_num(), begin, end);
	//			}
	//			return true;
	//		}
	//		get_next(begin, end);
	//	}
	//	return false;
	//}


	//bool parse_object(const char** begin, const char* end) {
	//	// only the key_value should be parsed in {}
	//	while (char ch = parser::get_cur_and_next(begin, end)) {
	//		if (ch == parser::json_key_symbol::object_begin || ch == parser::json_key_symbol::next_key_value) {
	//			parse_key_value(begin, end);
	//		}
	//		else if (ch == parser::json_key_symbol::object_end) {
	//			return true;
	//		}
	//	}
	//	return false;
	//}

	//size_t parse_array(const char** begin, const char* end) {
	//	// skip the white space and control char
	//	skip_space(begin, end);

	//	//check the value type
	//	if (**begin != json_key_symbol::array_begin) {
	//		check_skip(begin, end);
	//		return false;
	//	}

	//	int i = 0;
	//	while (char ch = get_cur_and_next(begin, end)) {
	//		// '[' and ',' as the falg of value begin
	//		if (ch == json_key_symbol::array_begin || ch == json_key_symbol::next_key_value) {
	//			skip_space(begin, end);

	//			if (**begin == json_key_symbol::array_end)
	//				return true;

	//			unserialize(begin, end);
	//			i++;
	//		}
	//		else if (ch == json_key_symbol::array_end) {
	//			return i;
	//		}
	//	}
	//	return i;
	//}
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

		// all number use 64bit, so remove the cl
		static inline length_t number_head_size() {
			return sizeof(head_t) - sizeof(length_t);
		}

		static inline length_t no_key_head_size() {
			return sizeof(head_t) - sizeof(key_t);
		}
		
		bool keycmp(const char* key) {
			if (!strcmp((const char*)this + head_size(), key)) {
				return true;
			}
			return false;
		}

		const char* get_key() {
			return (const char*)this + head_size();
		}

		void set_int(int num) {
			*(int*)((char*)this + number_head_size()) = num;
		}

		void set_string(const char* str,length_t len) {
			memcpy((char*)this + head_size() + kl + 1, str, len);
		}

		int get_int() {
			if (t == type_flag_t::num_t)
				return *(int*)((char*)this + number_head_size());
			return 0;
		}

		void get_string(string& str) {
			if (t == type_flag_t::str_t)
				str.assign((char*)this + head_size() + kl + 1, cl);
		}
	};
#pragma pack(pop)

#define ckeck_v if (!v) return *this

	struct json {
		const length_t head_size = sizeof(head_t);

		string data;
		head_t* h;

		json() {
		}

		head_t& push_head(flag_t t) {
			//add head
			length_t off = data.size();
			data.resize(data.size() + head_t::no_key_head_size());
			head_t &head = *(head_t*)(data.data() + off);
			head.t = t;
			return head;
		}

		head_t& push_head(flag_t t, const char* key, length_t kl) {
			//add head
			length_t off = data.size();
			data.resize(data.size() + head_t::head_size());
			head_t &head = *(head_t*)(data.data() + off);
			head.t = t;
			head.kl = kl;

			//add key end with '\0'
			push_str(key, kl);
			return head;
		}

		inline void push_str(const char* str,length_t len) {
			data.append(str, len);
			data.append(1, '\0');
		}

		json& operator [] (const char* key) {
			if (key && h) {
				// set this head as obj_t
				if (h->t == type_flag_t::pre_t) {
					h->t = type_flag_t::obj_t;
					h->cl = data.size();
					h = &push_head(type_flag_t::pre_t, key, strlen(key));
				}
				else{
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

		void operator = (int num) {
			if (h) {
				if (h->t == type_flag_t::num_t)
					h->set_int(num);
				else if (h->t == type_flag_t::pre_t) {
					h->t = type_flag_t::num_t;
					length_t head_off = (const char*)h - data.data();
					data.resize(data.size() + sizeof(number_t) - sizeof(key_t));
					h = (head_t*)(data.data() + head_off);
					h->set_int(num);
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
						h = &push_head(type_flag_t::str_t, h->get_key(), h->kl);
						h->cl = len;
						push_str(str, len);
					}
				}
				else if (h->t == type_flag_t::pre_t) {
					h->t = type_flag_t::str_t;
					h->cl = len;
					push_str(str, len);
				}
			}
		}

		int get_int(const char* key) {
			if (head_t *th = get_key_head(key))
				return th->get_int();
			return 0;
		}

		string get_string(const char* key) {
			string res;
			if (head_t *th = get_key_head(key))
				th->get_string(res);
			return res;
		}
	private:
		json(const json &v) {}

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
	}

	 int get_int(const char* key) {
		 json_data.h = (head_t*)json_data.data.data();
		 return json_data.get_int(key);
	 }

	 string get_string(const char* key) {
		 json_data.h = (head_t*)json_data.data.data();
		 return json_data.get_string(key);
	 }
public:
	// if *json end with '\0',don't need the size arg
	size_t unserialize(const char* json, size_t size = 0) {
		json_data.data.resize(0);
		const char* begin = json;
		const char* end = nullptr;
		if (size > 0)
			end = begin + size;
		//unserialize(&begin, end);
		return begin - json;
	}
};