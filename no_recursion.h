#pragma once

#include "dynamic.h"
#include <vector>
#include <stack>

struct info
{
	bool flag;
	int off;
};

class mystack {
public:
	info vec[50];
	int size = -1;
	info& push() {
		return vec[++size];
		//size++;
	}
	void pop() {
		size--;
	}
	info& top() {
		return vec[size];
	}
};

class no_recursion :public dynamic_json {
public:

	void skip_value(const char** begin, const char* end) {
		while (char ch = **begin) {
			if (ch == json_key_symbol::next_key_value || ch == json_key_symbol::object_end || ch == json_key_symbol::array_end) {
				return;
			}
			get_next(begin,end);
		}
	}

	char inline get_cur_and_next(const char** begin, const char* end) {
		return *((*begin)++);
	}

	void parse(const char** begin,const char* end) {
		skip_space(begin, end);
		mystack is_obj;
		while (char ch = parser::get_cur_and_next(begin, end)) {
			if (ch == parser::json_key_symbol::next_key_value) {
				//json_data.set_next(json_data.data.size());
				json_data.h->n = json_data.data.size();
			}
			else if (ch == parser::json_key_symbol::object_begin ) {
				size_t off = (const char*)json_data.h - json_data.data.data();
				json_data.set_flag(type_flag_t::obj_t);
				if (json_data.h->t != type_flag_t::obj_t) {
					json_data.push_head_and_set(type_flag_t::obj_t);
					json_data.set_child_or_length();
				}
				//else
					
				if (**begin == json_key_symbol::object_end) {
					get_next(begin, end);
					skip_space(begin, end);
					json_data.h->cl = 0;
					continue;
				}
				else {
 					is_obj.push() ={true,(int)off };
				}
			}
			else if (ch == parser::json_key_symbol::array_begin) {
				size_t off = (const char*)json_data.h - json_data.data.data();
				json_data.set_flag(type_flag_t::arr_t);
				if (json_data.h->t != type_flag_t::arr_t) {
					json_data.push_head_and_set(type_flag_t::arr_t);
					json_data.set_child_or_length();
				}
				//else
					
				if (**begin == json_key_symbol::array_end) {
					get_next(begin, end);
					skip_space(begin, end);
					json_data.h->cl = 0;
					continue;
				}
				else {
					is_obj.push() = { false,(int)off };
				}
			}
			else if (ch == parser::json_key_symbol::object_end) {
				json_data.set_next(is_obj.top().off);
				skip_space(begin, end);
				if (**begin != parser::json_key_symbol::next_key_value) {
					json_data.set_empty_next(is_obj.top().off);
				}
				else
					get_next(begin, end);
				is_obj.pop();
				continue;
			}
			else if (ch == parser::json_key_symbol::array_end) {
				json_data.set_next(is_obj.top().off);
				skip_space(begin, end);
				if (**begin != parser::json_key_symbol::next_key_value) {
					json_data.set_empty_next(is_obj.top().off);
					//get_next(begin, end);
				}
				else
					get_next(begin, end);
				is_obj.pop();
				continue;
			}

			skip_space(begin, end);
			if (is_obj.top().flag) {
				if (**begin == parser::json_key_symbol::str) {
					get_next(begin, end);
				}
				const char* b = *begin;
				parser::skip_str(begin, end);
				//string key(b, *begin - b - 1);
				//cout << key << ":";
		/*		if (key == "since_id_str") {
					cout << "err";
				}*/
				json_data.push_head_and_set(type_flag_t::pre_t, b, *begin - b - 1);
				skip_space(begin, end);
				if (**begin == json_key_symbol::key_value_separator) {
					get_next(begin, end);
				}
			}

			skip_space(begin, end);
			if (char ch = **begin) {
				if (ch == parser::json_key_symbol::str) {
					get_next(begin, end);
					const char* b = *begin;
					parser::skip_str(begin, end);
					//string key(b, *begin - b - 1);
					//cout << key << endl;
					json_data.push_str_and_set(b, *begin - b - 1);
				}
				else if (ch == parser::json_key_symbol::object_begin) {
					continue;
				}
				else if (ch == parser::json_key_symbol::array_begin) {
					continue;
				}
				else {
					//const char* b = *begin;
					//skip_value(begin, end);
					//string key(b, *begin - b);
					//cout << key << endl;
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
				}
			}
			skip_space(begin, end);
		}
	}

	size_t unserialize(const char* json, size_t size = 0) {
		json_data.data.resize(0);
		json_data.h = (head_t*)json_data.data.data();
		const char* begin = json;
		const char* end = nullptr;
		if (size > 0)
			end = begin + size;
		parse(&begin, end);
		return begin - json;
	}
};