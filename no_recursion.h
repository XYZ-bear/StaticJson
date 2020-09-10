#pragma once

#include "dynamic.h"
#include <vector>
#include <stack>

struct info
{
	bool flag;
	int off;
};

//class mystack {
//public:
//	info vec[50];
//	int i = -1;
//	void push(info in) {
//		vec[++i] = in;
//	}
//	info& top() {
//		return vec[i];
//	}
//	void pop() {
//		i--;
//	}
//};

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
};

class no_recursion :public dynamic_json {
public:
	char inline get_cur_and_next(const char** begin, const char* end) {
		return *((*begin)++);
	}

	void parse(const char** begin,const char* end) {
		//stack<info> is_obj;
		mystack is_obj;
		parser::skip_space(begin, end);

		if (**begin == parser::json_key_symbol::object_begin) {
			push_head_and_set(type_flag_t::obj_t);
			set_child_or_length();
		}
		else if(**begin == parser::json_key_symbol::array_begin) {
			push_head_and_set(type_flag_t::arr_t);
			set_child_or_length();
		}
		while (char ch = parser::get_cur_and_next(begin, end)) {
			if (ch == parser::json_key_symbol::next_key_value) {
				set_next();
				parser::skip_space(begin, end);
			}
			else if (ch == parser::json_key_symbol::object_begin ) {
			obj_begin:
				parser::skip_space(begin, end);
				if (**begin == parser::json_key_symbol::object_end) {
					parser::get_next(begin, end);
					parser::skip_space(begin, end);
					set_empty_child_or_length();
					continue;
				}

				//set the pre_t as obj_t
				set_flag(type_flag_t::obj_t);

				//record the offset of the cur head
				size_t off = get_off();

				//is_obj.push() ={true,(int)off };
				is_obj.push({ true,(int)off });
			}
			else if (ch == parser::json_key_symbol::array_begin) {
			arr_begin:
				parser::skip_space(begin, end);
				if (**begin == parser::json_key_symbol::array_end) {
					parser::get_next(begin, end);
					parser::skip_space(begin, end);
					set_empty_child_or_length();
					continue;
				}

				size_t off = get_off();
				set_flag(type_flag_t::arr_t);

				is_obj.push({ false,(int)off });
			}
			else if (ch == parser::json_key_symbol::object_end) {
			obj_end:
				parser::skip_space(begin, end);
				if (**begin != parser::json_key_symbol::next_key_value) {
					set_empty_next(is_obj.top().off);
				}
				else {
					set_next(is_obj.top().off);
					parser::get_next(begin, end);
				}
				is_obj.pop();
				continue;
			}
			else if (ch == parser::json_key_symbol::array_end) {
			arr_end:
				parser::skip_space(begin, end);
				if (**begin != parser::json_key_symbol::next_key_value) {
					set_empty_next(is_obj.top().off);
				}
				else {
					set_next(is_obj.top().off);
					parser::get_next(begin, end);
				}
				is_obj.pop();
				continue;
			}


			parser::skip_space(begin, end);
			//obj with key
			if (is_obj.top().flag) {
				if (**begin == parser::json_key_symbol::str) {
					parser::get_next(begin, end);
				}
				const char* b = *begin;
				parser::skip_str(begin, end);
				//string key(b, *begin - b - 1);
				//cout << key << ":";
		/*		if (key == "since_id_str") {
					cout << "err";
				}*/
				push_head_and_set(type_flag_t::pre_t, b, *begin - b - 1);
				parser::skip_space(begin, end);
				if (**begin == parser::json_key_symbol::key_value_separator) {
					parser::get_next(begin, end);
				}
			}
			else {
				push_head_and_set(type_flag_t::pre_t);
			}

			parser::skip_space(begin, end);
			if (char ch = **begin) {
				if (ch == parser::json_key_symbol::str) {
					parser::get_next(begin, end);
					const char* b = *begin;
					parser::skip_str(begin, end);
					//string key(b, *begin - b - 1);
					//cout << key << endl;
					push_str_and_set(b, *begin - b - 1);
				}
				else if (ch == parser::json_key_symbol::object_begin) {
					parser::get_next(begin, end);
					goto obj_begin;
					//continue;
				}
				else if (ch == parser::json_key_symbol::array_begin) {
					parser::get_next(begin, end);
					goto arr_begin;
					//continue;
				}
				else {
					//const char* b = *begin;
					//skip_value(begin, end);
					//string key(b, *begin - b);
					//cout << key << endl;
					if (ch == 'f') {
						*begin += 5;
						push_num(0);
					}
					else if (ch == 't' || ch == 'n') {
						*begin += 4;
						push_num(0);
					}
					else {
						parser::unserialize(&push_num(), begin, end);
					}
				}
			}
			parser::skip_space(begin, end);
		}
	}

	size_t unserialize(const char* json, size_t size = 0) {
		//data.resize(0);
		init();
		const char* begin = json;
		const char* end = nullptr;
		if (size > 0)
			end = begin + size;
		parse(&begin, end);
		return begin - json;
	}
};