#pragma once

#include <iostream>
#include <string>
#include <fstream> 
#include <iostream>
#include <unordered_map>
using namespace std;

template<class T>
struct data_imple_t {
	typedef size_t(T::*unserialize_t)(const char*, size_t);
	typedef void(T::*serialize_t)(string &res);
	unserialize_t unserialize;
	serialize_t serialize;
};

class colloect
{
public:
	template<class T>
	colloect(T *obj, const char* name, const data_imple_t<T> *field) {
		if (!obj->inited_)
			obj->__add_field(name, field);
	};
};

#define check_result(res)  if (!res) { return false; }

class field_op;

template<class T>
class json_base_t {
	enum json_key_symbol
	{
		object_begin = '{',
		object_end = '}',

		array_begin = '[',
		array_end = ']',

		key_value_separator = ':',
		next_key_value = ',',
		str = '"',
		space = ' '
	};
public:
	typedef T child_t;
	void __add_field(const char* name, const data_imple_t<T>* field) {
		if (field && fields_.find(name) == fields_.end()) {
			fields_[name] = field;
		}
		else {
			inited_ = true;
		}
	}
	json_base_t<T>& operator=(const json_base_t<T>& other) noexcept{
		return *this;
	}
	json_base_t() {
		//instance_; //模板类需要显示激活
		//fields_;
	}
	json_base_t(const json_base_t<T>& other) {
	}
protected:
private:
	static bool is_null(const char* begin, size_t len) {
		if (len == 4 && begin[0] == 'n' && begin[1] == 'u' && begin[2] == 'l' && begin[3] == 'l') {
			return true;
		}
		else
			return false;
	}
	static bool get_bool(const char* begin, size_t len) {
		if (len == 4 && begin[0] == 't' && begin[1] == 'r' && begin[2] == 'u' && begin[3] == 'e') {
			return true;
		}
		else if (len == 5 && begin[0] == 'f' && begin[1] == 'a' && begin[2] == 'l' && begin[3] == 's' && begin[3] == 'e') {
			return false;
		}
		else
			return false;
	}

	//check whether the string can pass the conversion
	//case 1: 1xx   case 2:-1xxx   pass no assert
	static bool check_can_convert_num(const char* begin, size_t len) {
		if (len > 0 && begin[0] >= '0' && begin[0] <= '9')
			return true;
		else if (len > 1 && begin[0] == '-' && begin[1] >= '0' && begin[1] <= '9')
			return true;
		else
			return false;
	}

	//escape the controll char like \t \r \f etc and whitespace
	static bool inline is_ctr_or_space_char(char ch) {
		return (ch == ' ' || (ch >= 0x00 && ch <= 0x1F) || ch == 0x7F);
	}
protected:
	inline static void serialize(bool *data, string &res) {
		if (*data)
			res += "true";
		else
			res += "false";
	}
	inline static void serialize(int64_t *data, string &res) {
		res += to_string(*data);
	}
	inline static void serialize(int32_t *data, string &res) {
		res += to_string(*data);
	}
	inline static void serialize(int16_t *data, string &res) {
		res += to_string(*data);
	}
	inline static void serialize(int8_t *data, string &res) {
		res += to_string(*data);
	}
	inline static void serialize(uint64_t *data, string &res) {
		res += to_string(*data);
	}
	inline static void serialize(uint32_t *data, string &res) {
		res += to_string(*data);
	}
	inline static void serialize(uint16_t *data, string &res) {
		res += to_string(*data);
	}
	inline static void serialize(uint8_t *data, string &res) {
		res += to_string(*data);
	}
	inline static void serialize(double *data, string &res) {
		res += to_string(*data);
	}
	inline static void serialize(string *data, string &res) {
		res += "\"" + *data + "\"";
	}
	inline static void serialize(float *data, string &res) {
		res += to_string(*data);
	}
	template<class T>
	inline static void serialize(vector<T> *data, string &res) {
		res += '[';
		for (auto &item : *data) {
			serialize(&item, res);
			res += ',';
		}
		if (data->size() > 0)
			res.pop_back();
		res += ']';
	}
	template<class T>
	inline static void serialize(json_base_t<T> *data, string &res) {
		data->serialize(res);
	}

	inline static size_t unserialize(bool *data, const char* begin, size_t len) {
		*data = get_bool(begin, len);
		return 0;
	}
	inline static size_t unserialize(int32_t *data, const char* begin, size_t len) {
		char* endptr;
		*data = strtol(begin, &endptr, 10);
		if (endptr != begin + len)
			*data = strtof(begin, &endptr);
		return 0;
	}
	inline static size_t unserialize(int8_t *data, const char* begin, size_t len) {
		char* endptr;
		*data = strtol(begin, &endptr, 10);
		if (endptr != begin + len)
			*data = strtof(begin, &endptr);
		return 0;
	}
	inline static size_t unserialize(int16_t *data, const char* begin, size_t len) {
		char* endptr;
		*data = strtol(begin, &endptr, 10);
		if (endptr != begin + len)
			*data = strtof(begin, &endptr);
		return 0;
	}
	inline static size_t unserialize(int64_t *data, const char* begin, size_t len) {
		char* endptr;
		*data = strtoll(begin, &endptr, 10);
		if (endptr != begin + len)
			*data = strtod(begin, &endptr);
		return 0;
	}
	inline static size_t unserialize(uint32_t *data, const char* begin, size_t len) {
		char* endptr;
		*data = strtoul(begin, &endptr, 10);
		if (endptr != begin + len)
			*data = strtod(begin, &endptr);
		return 0;
	}
	inline static size_t unserialize(uint8_t *data, const char* begin, size_t len) {
		char* endptr;
		*data = strtoul(begin, &endptr, 10);
		if (endptr != begin + len)
			*data = strtod(begin, &endptr);
		return 0;
	}
	inline static size_t unserialize(uint16_t *data, const char* begin, size_t len) {
		char* endptr;
		*data = strtoul(begin, &endptr, 10);
		if (endptr != begin + len) {
			*data = strtod(begin, &endptr);
		}
		return 0;
	}
	inline static size_t unserialize(uint64_t *data, const char* begin, size_t len) {
		char* endptr;
		*data = strtoull(begin, &endptr, 10);
		if (endptr != begin + len) {
			*data = strtod(begin, &endptr);
		}
		return 0;
	}

	inline static size_t unserialize(double *data, const char* begin, size_t len) {
		*data = strtod(begin, nullptr);
		return 0;
	}
	inline static size_t unserialize(float *data, const char* begin, size_t len) {
		*data = strtof(begin, nullptr);
		return 0;
	}
	inline static size_t unserialize(string *data, const char* begin, size_t len) {
		if (*begin == json_key_symbol::str)
			data->assign(begin + 1, len - 2);
		else if(!is_null(begin, len))
			data->assign(begin, len);
		return 0;
	}
	template<class V>
	inline static size_t unserialize(json_base_t<V> *data, const char* begin, size_t len) {
		return data->unserialize(begin, len);
	}

	// parse array with 4 cases
	// case 1:["xxx",...]
	// case 2:[xxx,...]
	// case 3:[{xxx},...]
	// case 4:[[xxx],...]
	template<class V>
	static size_t unserialize(vector<V> *data, const char* begin, size_t len) {
		const char* next = begin;
		const char* end = begin + len;
		const char* val_start = nullptr;

		while (char ch = get_next(&next,end)) {
			if (ch == json_key_symbol::array_end) {
				if (val_start) {
					V value;
					unserialize(&value, val_start, next - val_start);
					(*data).emplace_back(value);
				}
				return next - begin;
			}
			else if (ch == json_key_symbol::array_begin) {
				V value;
				next += unserialize(&value, next, end - next);
				(*data).emplace_back(value);
				val_start = nullptr;
			}  
			//pares string value
			else if (ch == json_key_symbol::str) {
				val_start = next - 1;
				check_result(parse_str(&next,end));
				V value;
				unserialize(&value, val_start, next - val_start);
				(*data).emplace_back(value);
				val_start = nullptr;
			}
			//pares value
			else if (val_start && (is_ctr_or_space_char(ch) || ch == json_key_symbol::next_key_value || ch == json_key_symbol::array_end )) {
				V value;
				unserialize(&value, val_start, next - val_start);
				(*data).emplace_back(value);
				val_start = nullptr;
			}
			else if (ch == json_key_symbol::object_begin) {
				val_start = next;
				V value;
				next += (unserialize(&value, next, end - next));
				(*data).emplace_back(value);
				val_start = nullptr;
			}
			else if (!val_start && (!is_ctr_or_space_char(ch) && ch != json_key_symbol::next_key_value && ch != json_key_symbol::object_end)) {
				val_start = next - 1;
			}
		}
		return 0;
	}

	// parse the string in quotes like "xxx",return the index of the last quote
	// case: "xxx"
	static bool parse_str(const char** begin, const char* end) {
		while (char ch = get_next(begin, end)) {
			if (ch == json_key_symbol::str) {
				if (*(*begin-2) !='\\')
					return true;
			}
		}
		return true;
	}

	// case 1:"xxx":xxx
	// case 2:"xxx":"xxx"
	// case 3:"xxx":{xxx}
	// case 4:"xxx":[xxx]
	bool parse_key_value(const char** begin, const char* end) {
		const char* key_start = *begin;
		parse_str(begin, end);
		const char* key_end = *begin;
		string key(key_start,*begin - 1);
		const char* val_start = nullptr;

		bool wait_val = false;
		while (char ch = get_next(begin, end)) {
			if (wait_val && ch == json_key_symbol::str) {
				val_start = *begin - 1;
				check_result(parse_str(begin, end));

				unserialize(key, val_start, *begin - val_start);
				return true;
			}
			// case 4:"xxx":[xxx]
			else if (wait_val && ch == json_key_symbol::array_begin) {
				*begin += unserialize(key, *begin, end - *begin);
				return true;
			}
			// case 3:"xxx":{xxx}
			else if (wait_val && ch == json_key_symbol::object_begin) {
				*begin += unserialize(key, *begin, end - *begin);
				return true;
			}
			// ready to pares value
			else if (ch == json_key_symbol::key_value_separator) {
				wait_val = true;
			}
			// get the first index of the value(escape white space and controll char)
			else if (wait_val && !val_start && !is_ctr_or_space_char(ch)) {
				val_start = *begin - 1;
			}
			// case 1:"xxx" : xxx
			else if (val_start && (is_ctr_or_space_char(ch) || ch == json_key_symbol::next_key_value || ch == json_key_symbol::object_end)) {
				unserialize(key, val_start, *begin - val_start - 1);
				return true;
			}
		}
		return false;
	}

	// pares object:
	// case 1:{"xxx":{xxx}}
	// return |      |len|| 
	// return |----len----|
	bool parse_object(const char** begin,const char* end) {
		// '{' must appear befor '}',set this as flag
		bool start = false;
		while (char ch = get_next(begin,end)) {
			// obj begin
			if (ch == json_key_symbol::object_begin) {
				if (start) {
					check_result(parse_object(begin,end));
				}
				else
					start = true;
			}
			else if (ch == json_key_symbol::object_end) {
				//if (!start) {
				//	cout << begin << ":error" << endl;
				//	return false;
				//}
				return true;
			}
			else if (ch == json_key_symbol::str) {
				check_result(parse_key_value(begin,end));
			}
			else if ( !is_ctr_or_space_char(ch)) {
				//if (!start) {
				//	return false;
				//}
			}
		}
		return false;
	}

	static char inline get_next(const char** begin,const char* end) {
		if (end && end < (*begin + 1))
			return '\0';
		else
			return *(*begin)++;
	}

	size_t unserialize(string &key, const char* val, size_t len) {
		auto iter = fields_.find(key);
		if (iter != fields_.end()) {
			return (((T*)this)->*(iter->second->unserialize))(val, len);
		}
		return 0;
	}
public:
	size_t unserialize(const char* json, size_t size = 0) {
		const char* begin = json;
		const char* end = nullptr;
		if (size > 0)
			end = begin + size;
		parse_object(&begin, end);

		return begin - json;
	}
	void serialize(string &res) {
		res += "{";
		for (auto &filed : fields_) {
			res += "\"" + filed.first + "\":";
			(((T*)this)->*(filed.second->serialize))(res);
			res += ',';
		}
		if (fields_.size() > 0)
			res.pop_back();
		res += "}";
	}
private:
	static std::unordered_map<string, const data_imple_t<T>*> fields_;
protected:
	static T instance_;
public:
	static bool inited_;
};
template<class T>
std::unordered_map<string, const data_imple_t<T>*> json_base_t<T>::fields_;
template<class T>
T json_base_t<T>::instance_;
template<class T>
bool json_base_t<T>::inited_ = false;


#define N(name) \
name;\
private:\
	size_t unserialize_##name( const char* begin,size_t len) {\
		return unserialize(&name,begin,len);\
	}\
	void serialize_##name(string &res) {\
		serialize(&name,res);\
	}\
	static constexpr data_imple_t<child_t> const_impl_##name = { &child_t::unserialize_##name,&child_t::serialize_##name };\
	colloect collect__##name{this,#name,&const_impl_##name};\
public:\

#define Json(name)\
class name :public json_base_t<name>