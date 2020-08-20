#pragma once

#include <iostream>
#include <string>
#include <fstream> 
#include <iostream>
#include <unordered_map>
using namespace std;

template<class T>
struct data_imple_t {
	typedef bool(T::*unserialize_t)(const char**, const char*);
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
	static bool parse_bool(bool &val, const char** begin, const char* end) {
		char t[5] = "true";
		char f[6] = "false";
		char index = 0;
		if (**begin == 't') {
			while (char ch = **begin) {
				if (index >= 4)
					break;
				else if (index < 4 && ch != t[index]) {
					val = false;
					return false;
				}
				index++;
				ch = get_next(begin, end);
			}
			val = true;
			return true;
		}
		else if (**begin == 'f') {
			while (char ch = **begin) {
				if (index >= 5)
					break;
				else if (index < 5 && ch != f[index]) {
					val = false;
					return false;
				}
				index++;
				ch = get_next(begin, end);
			}
			val = false;
			return true;
		}
		val = false;
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

	inline static bool unserialize(bool *data, const char** begin, const char* end) {
		if (!parse_bool(*data, begin, end)) {
			check_skip(begin, end);
			return false;
		}
		return true;
	}

	inline static bool unserialize(int32_t *data, const char** begin, const char* end) {
		char* endptr;
		*data = strtol(*begin, &endptr, 10);
		if (*endptr == '.')
			*data = strtof(*begin, &endptr);
		if (endptr == *begin) {
			check_skip(begin, end);
			return false;
		}
		(*begin) = endptr;
		return true;
	}
	inline static bool unserialize(int8_t *data, const char** begin, const char* end) {
		char* endptr;
		*data = strtol(*begin, &endptr, 10);
		if (*endptr == '.')
			*data = strtof(*begin, &endptr);
		if (endptr == *begin) {
			check_skip(begin, end);
			return false;
		}
		(*begin) = endptr;
		return true;
	}
	inline static bool unserialize(int16_t *data, const char** begin, const char* end) {
		char* endptr;
		*data = strtol(*begin, &endptr, 10);
		if (*endptr == '.')
			*data = strtof(*begin, &endptr);
		if (endptr == *begin) {
			check_skip(begin, end);
			return false;
		}
		(*begin) = endptr;
		return true;
	}
	inline static bool unserialize(int64_t *data, const char** begin, const char* end) {
		char* endptr;
		*data = strtoll(*begin, &endptr, 10);
		if (*endptr == '.')
			*data = strtod(*begin, &endptr);
		if (endptr == *begin) {
			check_skip(begin, end);
			return false;
		}
		(*begin) = endptr;
		return true;
	}
	inline static bool unserialize(uint32_t *data, const char** begin, const char* end) {
		char* endptr;
		*data = strtoul(*begin, &endptr, 10);
		if (*endptr == '.')
			*data = strtod(*begin, &endptr);
		if (endptr == *begin) {
			check_skip(begin, end);
			return false;
		}
		(*begin) = endptr;
		return true;
	}
	inline static bool unserialize(uint8_t *data, const char** begin, const char* end) {
		char* endptr;
		*data = strtoul(*begin, &endptr, 10);
		if (*endptr == '.')
			*data = strtod(*begin, &endptr);
		if (endptr == *begin) {
			check_skip(begin, end);
			return false;
		}
		(*begin) = endptr;
		return true;
	}
	inline static bool unserialize(uint16_t *data, const char** begin, const char* end) {
		char* endptr;
		*data = strtoul(*begin, &endptr, 10);
		if (*endptr == '.')
			*data = strtod(*begin, &endptr);
		if (endptr == *begin) {
			check_skip(begin, end);
			return false;
		}
		(*begin) = endptr;
		return true;
	}
	inline static bool unserialize(uint64_t *data, const char** begin, const char* end) {
		char* endptr;
		*data = strtoull(*begin, &endptr, 10);
		if (*endptr == '.')
			*data = strtod(*begin, &endptr);
		if (endptr == *begin) {
			check_skip(begin, end);
			return false;
		}
		(*begin) = endptr;
		return true;
	}

	inline static bool unserialize(double *data, const char** begin, const char* end) {
		char *endptr;
		*data = strtod(*begin, &endptr);
		if (endptr == *begin) {
			check_skip(begin, end);
			return false;
		}
		(*begin) = endptr;
		return true;
	}
	inline static bool unserialize(float *data, const char** begin, const char* end) {
		char *endptr;
		*data = strtof(*begin, &endptr);
		if (endptr == *begin) {
			check_skip(begin, end);
			return false;
		}
		(*begin) = endptr;
		return true;
	}
	inline static bool unserialize(string *data, const char** begin, const char* end) {
		if (get_cur_and_next(begin, end) == json_key_symbol::str) {
			*data = "";
			parse_str(*data, begin, end);
		}
		else {
			check_skip(begin, end);
			return false;
		}
		return true;
	}
	template<class V>
	inline static bool unserialize(json_base_t<V> *data, const char** begin, const char* end) {
		if (**begin == json_key_symbol::object_begin) {
			if (end)
				*begin += data->unserialize(*begin, end - *begin);
			else
				*begin += data->unserialize(*begin);
		}
		else {
			check_skip(begin, end);
			return false;
		}
		return true;
	}

	// parse array with 4 cases
	// case 1:["xxx",...]
	// case 2:[xxx,...]
	// case 3:[{xxx},...]
	// case 4:[[xxx],...]
	template<class V>
	static bool unserialize(vector<V> *data, const char** begin, const char* end) {
		skip_space(begin, end);

		if (**begin != json_key_symbol::array_begin) {
			check_skip(begin, end);
			return false;
		}

		while (char ch = get_cur_and_next(begin, end)) {
			if (ch == json_key_symbol::array_begin || ch == json_key_symbol::next_key_value) {
				skip_space(begin, end);
				V value;
				if (unserialize(&value, begin, end))
					(*data).emplace_back(value);
			}
			else if (ch == json_key_symbol::array_end) {
				return true;
			}
		}
		return false;
	}

	// parse the string in quotes like "xxx",return the index of the last quote
	// case: "xxx"
	static bool parse_str(string &val, const char** begin, const char* end) {
		char pre_ch;
		while (char ch = get_cur_and_next(begin, end)) {
			if (ch == json_key_symbol::str && pre_ch != '\\') {
				return true;
			}
			val += ch;
			pre_ch = ch;
		}
		return true;
	}

	// case 1:"xxx":xxx
	// case 2:"xxx":"xxx"
	// case 3:"xxx":{xxx}
	// case 4:"xxx":[xxx]
	bool parse_key_value(const char** begin, const char* end) {
		skip_space(begin, end);
		if (get_cur_and_next(begin,end) == json_key_symbol::str) {
			//get key
			string key;
			parse_str(key, begin, end);

			//parse value
			skip_space(begin, end);
			if (get_cur_and_next(begin, end) == json_key_symbol::key_value_separator) {
				skip_space(begin, end);
				return unserialize(key, begin, end);
			}
			else
				return false;
		}
		return false;
	}

	// pares object:
	// case 1:{"xxx":{xxx}}
	// return |      |len|| 
	// return |----len----|
	bool parse_object(const char** begin,const char* end) {
		// only the key_value should be parsed in {}
		while (char ch = get_cur_and_next(begin, end)) {
			if (ch == json_key_symbol::object_begin || ch == json_key_symbol::next_key_value) {
				parse_key_value(begin, end);
			}
			else if (ch == json_key_symbol::object_end) {
				return true;
			}
		}
		return false;
	}

	static void check_skip(const char** begin, const char* end) {
		char ch = get_cur_and_next(begin, end);
		if (ch == json_key_symbol::object_begin) {
			skip_object(begin, end);
		}
		else if (ch == json_key_symbol::array_begin) {
			skip_array(begin, end);
		}
		else if (ch == json_key_symbol::str) {
			skip_str(begin, end);
		}
	}

	static void skip_object(const char** begin, const char* end) {
		while (char ch = get_cur_and_next(begin, end)) {
			if (ch == json_key_symbol::object_begin)
				skip_object(begin, end);
			else if (ch == json_key_symbol::object_end)
				return;
		}
	}

	static void skip_array(const char** begin, const char* end) {
		while (char ch = get_cur_and_next(begin, end)) {
			if (ch == json_key_symbol::array_begin)
				skip_array(begin, end);
			else if (ch == json_key_symbol::array_end)
				return;
		}
	}

	static void skip_str(const char** begin, const char* end) {
		char pre_ch;
		while (char ch = get_cur_and_next(begin, end)) {
			if (ch == json_key_symbol::str && pre_ch != '\\') {
				return;
			}
			pre_ch = ch;
		}	
	}

	static void skip_space(const char** begin,const char* end) {
		while (char ch = **begin) {
			if (!is_ctr_or_space_char(ch))
				return;
			ch = get_next(begin, end);
		}
	}

	static char inline get_cur_and_next(const char** begin,const char* end) {
		if (end && end < (*begin+1) )
			return '\0';
		else
			return *((*begin)++);
	}

	static char inline get_next(const char** begin, const char* end) {
		(*begin)++;
		if (end && end < *begin)
			return '\0';
		else
			return **begin;
	}

	size_t unserialize(string &key, const char** val, const char* end) {
		auto iter = fields_.find(key);
		if (iter != fields_.end()) {
			return (((T*)this)->*(iter->second->unserialize))(val, end);
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
	bool unserialize_##name( const char** begin,const char* end) {\
		return unserialize(&name,begin,end);\
	}\
	void serialize_##name(string &res) {\
		serialize(&name,res);\
	}\
	static constexpr data_imple_t<child_t> const_impl_##name = { &child_t::unserialize_##name,&child_t::serialize_##name };\
	colloect collect__##name{this,#name,&const_impl_##name};\
public:\

#define Json(name)\
class name :public json_base_t<name>