#pragma once

#include <iostream>
#include <string>
#include <fstream> 
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>

#define IEEE_8087

#ifdef IEEE_8087
#include "dtoa.c"
#endif //IEEE_8087


using namespace std;

#define Json(name)\
class name :public json_base<name>

#define N(name)																														\
name;																																\
private:																															\
	bool unserialize_##name( const char** begin,const char* end) {																	\
		return unserialize(&name,begin,end);																						\
	}																																\
	void serialize_##name(string &res) {																							\
		serialize(&name,res);																										\
	}																																\
	class class__##name:private static_instance<class__##name>{																		\
	public:																															\
		class__##name() {																											\
			data_impl_t<child_t> fie={&child_t::unserialize_##name,&child_t::serialize_##name};										\
			field_collector<child_t>::fields(#name,&fie);																			\
		}																															\
	};																																\
public:


#define STRNG_TO_NUM(data_t,methodl,methodf)	\
char* endptr;									\
*data = (data_t)methodl(*begin, &endptr, 10);	\
if (*endptr == '.')								\
	*data = (data_t)methodf(*begin, &endptr);	\
if (endptr == *begin) {							\
	check_skip(begin, end);						\
	return false;								\
}												\
(*begin) = endptr;								\
return true;									\


template<class T>
struct data_impl_t {
	typedef bool(T::*unserialize_t)(const char**, const char*);
	typedef void(T::*serialize_t)(string &res);
	unserialize_t unserialize;
	serialize_t serialize;
};

template<class T>
class static_instance {
public:
	static_instance() {
		ins;
	}
private:
	static T ins;
};
template<class T>
T static_instance<T>::ins;

class Key {
public:
	const char* str;
	size_t len;

	Key(const char* s, size_t l = 0) {
		str = s;
		len = l;
	}
};

namespace std {
	template<>
	struct hash<Key> {//哈希的模板定制
	public:
		size_t operator()(const Key &p) const
		{
			unsigned long h = 0;
			if (p.len) {
				for (int i = 0; i < p.len; i++) {
					h = 5 * h + p.str[i];
				}
			}
			else {
				const char *s = p.str;
				for (; *s; ++s)
					h = 5 * h + *s;
			}
			return size_t(h);
			//return hash<string>()(p.name) ^ hash<int>()(p.age);
		}

	};

	template<>
	struct equal_to<Key> {//等比的模板定制
	public:
		bool operator()(const Key &p1, const Key &p2) const
		{
			for (int i = 0; (p1.str[i] || i < p1.len) && (p2.str[i] || i < p2.len); i++) {
				if (p1.str[i] != p2.str[i])
					return false;
			}
			return true;
		}

	};
}
//#define KKey

#ifdef KKey
template<class T>
class field_collector {
public:
	static const std::unordered_map<Key, data_impl_t<T>>& fields(const char* name = nullptr, const data_impl_t<T> *field = nullptr) {
		static std::unordered_map<Key, data_impl_t<T>> fields;
		if (field && fields.find(Key(name)) == fields.end()) {
			fields[Key(name)] = *field;
		}
		return fields;
	}
};
#else
template<class T>
class field_collector {
public:
	static const std::unordered_map<string, data_impl_t<T>>& fields(const char* name = nullptr, const data_impl_t<T> *field = nullptr) {
		static std::unordered_map<string, data_impl_t<T>> fields;
		if (field && fields.find(name) == fields.end()) {
			fields[name] = *field;
		}
		return fields;
	}
};
#endif // KKey




template<class T>
class json_base{
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
private:
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
		else {
			char* endptr;
			val = strtol(*begin, &endptr, 10);
			if (*endptr == '.')
				val = strtof(*begin, &endptr);
			if (endptr == *begin)
				return false;
			else
				return true;
		}
		val = false;
		return false;
	}

	//escape the controll char like \t \r \f etc and whitespace
	static bool inline is_ctr_or_space_char(char ch) {
		return (ch == ' ' || (ch >= 0x00 && ch <= 0x1F) || ch == 0x7F);
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

	static size_t get_array_size(const char* begin, const char* end) {
		int size = 0;
		while (char ch = get_cur_and_next(&begin, end)) {
			if (ch == json_key_symbol::array_begin) {
				skip_array(&begin, end);
			}
			else if (ch == json_key_symbol::object_begin) {
				skip_object(&begin, end);
			}
			else if (ch == json_key_symbol::next_key_value) {
				size++;
			}
			else if (ch == json_key_symbol::array_end)
				return ++size;
		}
		return 0;
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

	static void skip_space(const char** begin, const char* end) {
		while (char ch = **begin) {
			if (!is_ctr_or_space_char(ch))
				return;
			ch = get_next(begin, end);
		}
	}

	static char inline get_cur_and_next(const char** begin, const char* end) {
		if (end && end < (*begin + 1))
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
#ifdef IEEE_8087
		int si=0;
		int de=0;

		const char* re = dtoa(*data, 0, 0, &de, &si, 0);
		if (si)
			res += '-';
		if (de>0) {
			res.append(re, de);
			res += '.';
			res += (re + de);
		}
		else if (de < 0) {
			res += "0.";
			while (0 != de++) {
				res += '0';
			}
			res += re;
		}
		
#else
		res += to_string(*data);
#endif // IEEE_8087
	}
	inline static void serialize(string *data, string &res) {
		res += "\"" + *data + "\"";
	}
	inline static void serialize(float *data, string &res) {
		res += to_string(*data);
	}
	template<class V>
	inline static void serialize(vector<V> *data, string &res) {
		res += json_key_symbol::array_begin;
		for (auto &item : *data) {
			serialize(&item, res);
			res += json_key_symbol::next_key_value;
		}
		if (data->size() > 0)
			res.pop_back();
		res += json_key_symbol::array_end;
	}
	template<class V>
	inline static void serialize(json_base<V> *data, string &res) {
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
		STRNG_TO_NUM(int32_t,strtol, strtof);
	}
	inline static bool unserialize(int8_t *data, const char** begin, const char* end) {
		STRNG_TO_NUM(int8_t,strtol, strtof);
		return true;
	}
	inline static bool unserialize(int16_t *data, const char** begin, const char* end) {
		STRNG_TO_NUM(int16_t,strtol, strtof);
		return true;
	}
	inline static bool unserialize(int64_t *data, const char** begin, const char* end) {
		STRNG_TO_NUM(int64_t,strtoll, strtod);
	}
	inline static bool unserialize(uint32_t *data, const char** begin, const char* end) {
		STRNG_TO_NUM(uint32_t,strtoul, strtod);
	}
	inline static bool unserialize(uint8_t *data, const char** begin, const char* end) {
		STRNG_TO_NUM(uint8_t,strtoul, strtod);
	}
	inline static bool unserialize(uint16_t *data, const char** begin, const char* end) {
		STRNG_TO_NUM(uint16_t,strtoul, strtod);
	}
	inline static bool unserialize(uint64_t *data, const char** begin, const char* end) {
		STRNG_TO_NUM(uint64_t,strtoull, strtod);
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
			parse_str(*data, begin, end);
		}
		else {
			check_skip(begin, end);
			return false;
		}
		return true;
	}
	template<class V>
	inline static bool unserialize(json_base<V> *data, const char** begin, const char* end) {
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

	template<class V>
	static bool unserialize(vector<V> *data, const char** begin, const char* end) {
		// skip the white space and control char
		skip_space(begin, end);

		//check the value type
		if (**begin != json_key_symbol::array_begin) {
			check_skip(begin, end);
			return false;
		}

		data->reserve(2);
		while (char ch = get_cur_and_next(begin, end)) {
			// '[' and ',' as the falg of value begin
			if (ch == json_key_symbol::array_begin || ch == json_key_symbol::next_key_value) {
				skip_space(begin, end);

				if (**begin == json_key_symbol::array_end)
					return true;

				size_t index = data->size();
				data->resize(index + 1);
				unserialize(&((*data)[index]), begin, end);

			}
			else if (ch == json_key_symbol::array_end) {
				return true;
			}
		}
		return false;
	}

	// end with '"' and skip "\""
	static bool parse_str(string &val, const char** begin, const char* end) {
		const char* b = *begin;
		char pre_ch = **begin;
		while (char ch = get_cur_and_next(begin, end)) {
			if (ch == json_key_symbol::str && pre_ch != '\\') {
				val.assign(b, (*begin)-1);
				return true;
			}
			pre_ch = ch;
		}
		return false;
	}

	// case 1:"xxx":xxx
	// case 2:"xxx":"xxx"
	// case 3:"xxx":{xxx}
	// case 4:"xxx":[xxx]
#ifdef KKey
	bool parse_key_value(const char** begin, const char* end) {
		skip_space(begin, end);
		if (get_cur_and_next(begin,end) == json_key_symbol::str) {
			const char* b = *begin;
			skip_str(begin, end);
			//size_t ha = hash_str( begin, end);
			Key key(b, (*begin) - 1 - b);
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
#else
	bool parse_key_value(const char** begin, const char* end) {
		skip_space(begin, end);
		if (get_cur_and_next(begin, end) == json_key_symbol::str) {
			const char* b = *begin;
			skip_str(begin, end);
			string key(b , (*begin) - 1);
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
#endif

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

#ifdef KKey
	bool unserialize(Key &key, const char** val, const char* end) {
		auto &fields = field_collector<child_t>::fields();
		auto iter = fields.find(key);
		if (iter != fields.end()) {
			return (((T*)this)->*(iter->second.unserialize))(val, end);
		}
		check_skip(val, end);
		return false;
	}

#else
	bool unserialize(string &key, const char** val, const char* end) {
		auto &fields = field_collector<child_t>::fields();
		auto iter = fields.find(key);
		if (iter != fields.end()) {
			return (((T*)this)->*(iter->second.unserialize))(val, end);
		}
		check_skip(val, end);
		return false;
	}
#endif // KKey
public:
	// if *json end with '\0',don't need the size arg
	size_t unserialize(const char* json, size_t size = 0) {
		const char* begin = json;
		const char* end = nullptr;
		if (size > 0)
			end = begin + size;
		parse_object(&begin, end);

		return begin - json;
	}

	void serialize(string &res) {
		//auto &fields = field_collector<child_t>::fields();
		//res += json_key_symbol::object_begin;
		//for (auto &filed : fields) {
		//	res += "\"" + filed.first + "\":";
		//	(((T*)this)->*(filed.second.serialize))(res);
		//	res += json_key_symbol::next_key_value;
		//}
		////pop the json_key_symbol::next_key_value(',')
		//if (fields.size() > 0)
		//	res.pop_back();
		//res += json_key_symbol::object_end;
	}
};

