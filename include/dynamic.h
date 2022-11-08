#pragma once
#include "static_json.h"
#include <vector>
#include <functional>
#include <map>

#ifdef unix
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#endif
/*Linear chain storage structure
Linear: Physical Structure,line memory space,like vector,
Chain: Logical Structure, the nodes linked by logical

In this structure, we can estimate the size of space in advance by the json length, and release the space once time
When visit the nodes, we will get a high memeory catch hit rate.

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

//! Array - based stack structure
class json_stack {
public:
	//! Used to record stack information
	struct info
	{
		bool is_obj;
		int off;
	};
	vector<info> vec;
	json_stack() {
		vec.reserve(6);
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

//! json_value json_iteratorator
//! \tparam T the type of json_value
template <class T>
class json_iterator
{
public:
	json_iterator(T* pp, bool o, int off) :p(pp), end(o) {
		begin = off;
	}

	json_iterator(const json_iterator<T>& i) :p(i.p) {}

	json_iterator<T>& operator = (const json_iterator<T>& i) { p = i.p; return *this; }

	bool operator != (const json_iterator<T>& i) {
		if (end == i.end)
			return false;
		return true;
	}

	T& operator *() { return *p; }

	T& operator ++() {
		end = p->next(&iter_stack, begin);
		return *p;
	}

	T& next_brother() {
		end = p->next_brother(begin);
		return *p;
	}
private:
	T* p;
	json_stack iter_stack;
	bool end;
	int begin;
};

static vector<size_t> prime_list = { 53 , 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241, 786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741 };

template<class TYPE>
struct template_param
{
	static TYPE b;
};

#pragma pack(push,1)
struct hash_node
{
	uint32_t next = 0;
	uint32_t parent = 0;
	uint32_t value_off = 0;
};

struct shm_pack {
	size_t _data_shmid = -1;
	size_t _data_count = 0;
	size_t _hash_shmid = -1;
	size_t _hash_count = 0;;
	size_t _link_shmid = -1;
	size_t _link_count = 0;;
	size_t _other_shmid = -1;
	size_t _other_count = 0;;
};
#pragma pack(pop)

#ifdef unix
template <typename T>
class shm_allocator : public std::allocator<T>
{
public:
	typedef size_t   size_type;
	typedef typename std::allocator<T>::pointer              pointer;
	typedef typename std::allocator<T>::value_type           value_type;
	typedef typename std::allocator<T>::const_pointer        const_pointer;
	typedef typename std::allocator<T>::reference            reference;
	typedef typename std::allocator<T>::const_reference      const_reference;

	pointer allocate(size_type _Count, const void* _Hint = NULL) {
		if (!init_ptr) {
			*count = _Count;
			shmctl(*shmid, IPC_RMID, 0);
			*shmid = shmget(IPC_PRIVATE, (size_t)(_Count * sizeof(T)), 0666 | IPC_CREAT);
			return (pointer)shmat(*shmid, 0, 0);
		}
		auto t = init_ptr;
		init_ptr = nullptr;
		return t;

	}

	void deallocate(pointer _Ptr, size_type _Count) {
		shmdt((const void*)_Ptr);
	}

	void release() {
		shmctl(*shmid, IPC_RMID, 0);
	}

	template<class _Other>
	struct rebind {
		// convert this type to allocator<_Other>  
		typedef shm_allocator<_Other> other;
	};

	shm_allocator() throw() {
		shmid = new size_t(-1);
		count = new size_t(0);
	}

	shm_allocator(size_t* shmid, size_t* count) throw() {
		this->shmid = shmid;
		this->count = count;
		if (*shmid == 0)
			*shmid = -1;
		else {
			init_ptr = (pointer)shmat(*shmid, 0, 0);
		}
	}

	shm_allocator(const shm_allocator& __a) throw()
		: std::allocator<T>(__a) {
		this->shmid = __a.shmid;
		this->init_ptr = __a.init_ptr;
		this->count = __a.count;
	}

	template<typename _Tp1>
	shm_allocator(const shm_allocator<_Tp1>& __a) throw() {
		this->shmid = __a.shmid;
		this->init_ptr = __a.init_ptr;
		this->count = __a.count;
	}

	size_t get_shmid() {
		return *shmid;
	}

	~shm_allocator() throw() {}

	size_t* shmid = nullptr;
	size_t* count = nullptr;
	pointer init_ptr = nullptr;
};
#endif

template <typename T>
class std_allocator : public std::allocator<T>
{
public:
	typedef size_t   size_type;
	typedef typename std::allocator<T>::pointer              pointer;
	typedef typename std::allocator<T>::value_type           value_type;
	typedef typename std::allocator<T>::const_pointer        const_pointer;
	typedef typename std::allocator<T>::reference            reference;
	typedef typename std::allocator<T>::const_reference      const_reference;

	template<class _Other>
	struct rebind {
		typedef std_allocator<_Other> other;
	};

	std_allocator() throw() {
	}

	std_allocator(size_t* shmid, size_t* count) throw() : std::allocator<T>() {
	}

	void release() {
	}

	std_allocator(const std_allocator& __a) throw()
		: std::allocator<T>(__a) {
	}

	template<typename _Tp1>
	std_allocator(const std_allocator<_Tp1>& __a) throw() {
	}

	size_t get_shmid() {
		return 0;
	}
};

class mmp_op {
public:
	static void* allocate(const char* key, size_t size) {
#ifdef unix
		int pack_id = shm_open(key, O_CREAT | O_RDWR, 0666);
		ftruncate(pack_id, size);
		return mmap(0, size, PROT_WRITE, MAP_SHARED, pack_id, 0);
#else
		return nullptr;
#endif
	}

	static void release(const char* key) {
#ifdef unix
		shm_unlink(key);
#endif
	}

	static bool exist(const char* key) {
#ifdef unix
		auto pack_id = shm_open(key, O_CREAT | O_RDWR, 0666);
		if (pack_id < 0) {
			release(key);
			return false;
		}
		ftruncate(pack_id, sizeof(shm_pack));
		shm_pack* pack = (shm_pack*)mmap(0, sizeof(shm_pack), PROT_WRITE, MAP_SHARED, pack_id, 0);
		if (shmctl(pack->_data_shmid, IPC_STAT, 0) < 0) {
			release(key);
			return false;
		}
		if (shmctl(pack->_hash_shmid, IPC_STAT, 0) < 0) {
			release(key);
			return false;
		}
		if (shmctl(pack->_link_shmid, IPC_STAT, 0) < 0) {
			release(key);
			return false;
		}
		if (shmctl(pack->_other_shmid, IPC_STAT, 0) < 0) {
			release(key);
			return false;
		}
		return true;
#endif
		return true;
	}

};

typedef std_allocator<char> d_alloc_t;
typedef std_allocator<hash_node> h_alloc_t;
#ifdef unix
typedef shm_allocator<char> shm_data_alloc_t;
typedef shm_allocator<hash_node> shm_hash_alloc_t;
#else
typedef std_allocator<char> shm_data_alloc_t;
typedef std_allocator<hash_node> shm_hash_alloc_t;
#endif

//! an efficient array-linked structure
template<class DATA_ALLOC = d_alloc_t, class HASH_ALLOC = h_alloc_t>
struct json_value {
public:
	//! inorder to support old compiler, separeate with type_flag_t
	typedef char flag_t;

	//! point to the next value
	typedef uint32_t next_t;

	//! by default, the json value memery size just support 4G
	typedef uint32_t length_t;

	//! just support 8byte length number
	typedef uint64_t number_t;

	//! when the key length <= 255, then store the key length
	//  if key length > 255, then store the length as 256, use strlen to get the real length
	typedef uint8_t key_t;

	typedef std::basic_string<char, std::char_traits<char>, DATA_ALLOC> data_t;

	typedef std::vector<hash_node, HASH_ALLOC> hash_table_t;

	typedef std::vector<hash_node, HASH_ALLOC> link_table_t;

	//! all the json token type
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
		del_t = 8,
		num_double_t = 9,
		num_int_t = 10
	};

	//! help you to push the childs into a vector
	struct vector_helper {
		json_value* root;
		vector<int> vec;
		json_value& operator [] (int index) {
			if (root && index < vec.size())
				return root->to_off(vec[index]);
			return *root;
		}
	};

	//! help you to push the childs into a map with key
	struct map_helper {
		json_value* root;
		unordered_map<no_copy_string, length_t> mapp;
		json_value& operator [] (const char* key) {
			if (root)
				return root->to_off(mapp[key]);
			return *root;
		}
	};

	template<class PTR>
	struct offset_ptr
	{
		data_t* ptr = nullptr;
		size_t offset = 0;
		offset_ptr() {
		}
		offset_ptr(data_t* p, size_t o) {
			ptr = p;
			offset = o;
		}
		offset_ptr(const offset_ptr<PTR>& l) {
			ptr = l.ptr;
			offset = l.offset;
		}
		inline void update(size_t off) {
			offset = off;
		}
		inline void update(data_t* p, size_t o) {
			ptr = p;
			offset = o;
		}
		inline void operator = (PTR* p) {
			offset = (const char*)p - (const char*)ptr->data();
		}
		inline bool operator != (PTR* p) {
			return p != (PTR*)(ptr->data() + offset);
		}
		inline bool operator == (PTR* p) {
			return p == (PTR*)(ptr->data() + offset);
		}
		inline bool operator == (nullptr_t p) const {
			return ptr == p;
		}
		inline operator bool()const noexcept {
			return ptr;
		}
		inline operator const char* ()const noexcept {
			return (const char*)(ptr->data() + offset);
		}
		inline operator PTR* ()const noexcept {
			return (PTR*)(ptr->data() + offset);
		}
		inline PTR* operator -> () const {
			return (PTR*)(ptr->data() + offset);
		}
		inline void refresh() {
			offset = ptr->size();
		}
	};

protected:
#pragma pack(push,1)
	//! Head interpreter
	struct head_t {
		//! type
		flag_t t;

		//! next node offset
		next_t n;

		next_t p;

		//! t == arr_t or obj_t -> cl = child offset
		//  t == num_t -> cl = sub_type (num_int_t or num_double_t)
		//  t == str_t -> cl = strlen
		length_t cl;

		//! key length
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

		bool keycmp(const char* key) {
			const char* fk = ((const char*)this + head_size());
			uint8_t i = 0;
			uint8_t e = kl;
			for (uint8_t i = 0; i <= kl / 2; i++, --e) {
				if (fk[i] != key[i] || fk[e] != key[e])
					return false;
			}
			return true;
			if (!strcmp(fk, key)) {
				return true;
			}
			return false;
		}

		const char* get_key() {
			return (const char*)this + head_size();
		}

		char* get_val() {
			if (kl < 255)
				return kl == 0 ? (char*)this + head_size() + kl : (char*)this + head_size() + kl + 1;
			return (char*)this + head_size() + strlen(get_key()) + 1;

			//return kl == 0 ? (char*)this + head_size() + kl : (char*)this + head_size() + strlen(get_key()) + 1;
		}

		template<class N>
		void set_num(N num) {
			*(N*)(get_val()) = num;
		}

		void set_string(const char* str, length_t len) {
			memcpy(get_val(), str, len);
		}

		template<class N>
		N get_num(template_param<N> j) {
			if (t == type_flag_t::num_t || type_flag_t::boo_t)
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

	typedef offset_ptr<head_t> head_ptr_t;
#pragma pack(pop)
public:
	json_value() {
		data = new data_t();
		update_head(0);
		h = head_ptr_t(data, 0);
		od = data->data();

		ph = 0;
		last_push = 0;

		hash_table = new hash_table_t();
		link_table = new link_table_t();
		other_table = new link_table_t(1);
		mode_index = &(*other_table)[0].next;
		count = &(*other_table)[0].parent;
		hash_table->resize(prime_list[*mode_index]);
	}

	json_value(const char* key) {
		mmp_op::exist(key);
#ifdef unix
		shm_pack* pack = (shm_pack*)mmp_op::allocate(key, sizeof(shm_pack));
		if (pack->_data_shmid) {
			init_from_pack(pack);
			return;
		}
#else
		shm_pack packt;
		shm_pack* pack = &packt;
#endif // unix



		data = new data_t(DATA_ALLOC(&pack->_data_shmid, &pack->_data_count));
		update_head(0);
		h = head_ptr_t(data, 0);
		od = data->data();

		ph = 0;
		last_push = 0;

		hash_table = new hash_table_t(HASH_ALLOC(&pack->_hash_shmid, &pack->_hash_count));
		link_table = new link_table_t(HASH_ALLOC(&pack->_link_shmid, &pack->_link_count));
		other_table = new link_table_t(HASH_ALLOC(&pack->_other_shmid, &pack->_other_count));
		other_table->resize(1);
		mode_index = &(*other_table)[0].next;
		count = &(*other_table)[0].parent;
		hash_table->resize(prime_list[*mode_index]);
	}

	void init_from_pack(shm_pack* pack) {
		data = new data_t(DATA_ALLOC(&pack->_data_shmid, &pack->_data_count));
		data->reserve(pack->_data_count);
		update_head(0);
		h = head_ptr_t(data, 0);
		od = data->data();

		ph = 0;
		last_push = 0;
		hash_table = new hash_table_t(HASH_ALLOC(&pack->_hash_shmid, &pack->_hash_count));
		link_table = new link_table_t(HASH_ALLOC(&pack->_link_shmid, &pack->_link_count));
		other_table = new link_table_t(HASH_ALLOC(&pack->_other_shmid, &pack->_other_count));
		hash_table->reserve(pack->_hash_count);
		link_table->reserve(pack->_link_count);
		other_table->reserve(pack->_other_count);
		mode_index = &(*other_table)[0].next;
		count = &(*other_table)[0].parent;
	}

	shm_pack get_shm_pack() {
		return { data->get_allocator().get_shmid(), data->size(),
				 hash_table->get_allocator().get_shmid(), hash_table->size(),
				 link_table->get_allocator().get_shmid(), link_table->size(),
				 other_table->get_allocator().get_shmid(), other_table->size()
		};
	}

	~json_value() {
	}

	inline data_t& gdata() {
		return *data;
	}

	inline size_t gdata1() {
		return hash_table->size() * sizeof(hash_node) + link_table->size() * sizeof(hash_node);
	}

	void release() {
		delete data;
		delete hash_table;
		delete link_table;
		delete other_table;
	}

	void release_shm(const char* key) {
		if (data && hash_table && hash_table) {
			mmp_op::release(key);
			data->get_allocator().release();
			hash_table->get_allocator().release();
			link_table->get_allocator().release();
			other_table->get_allocator().release();
		}
	}

	//! the begin json_iteratorator
	json_iterator<json_value> begin() {
		length_t off = get_off();
		if (h->t == type_flag_t::obj_t || h->t == type_flag_t::arr_t) {
			update_head(h->cl);
		}
		return json_iterator<json_value>(this, false, off);
	}

	//! the end json_iteratorator
	json_iterator<json_value> end() {

		return json_iterator<json_value>(this, true, 0);
	}

	//! get object value
	json_value operator [] (const char* key) {

		if (key && h) {
			// set this head as obj_t
			if (h->t == type_flag_t::pre_t) {
				ph = get_off();
				h->t = type_flag_t::obj_t;
				h->cl = (length_t)data->size();
				push_head(type_flag_t::pre_t, key, (length_t)strlen(key));
			}
			else {
				// dont find the head -> add this key
				if (hash_node* node = find_node(key)) {
					head_ptr_t th(data, node->value_off);
					if (th->t == type_flag_t::obj_t)
						return json_value(*this, th, node->value_off);
					else
						return json_value(*this, th, ph);
				}

				//bool is_find = false;
				//head_t *th = get_key_head(key, is_find);
				//if (is_find) {
				//	return json_value(*this, th, length_t((const char*)th - data->data()));
				//}
				// 
				//head_t* th = get_end_head();
				//if (!th) {
				//	push_head(type_flag_t::obj_t);
				//	h->cl = (length_t)data->size();
				//}


				head_t* th = nullptr;
				if (last_push == 0) {
					push_head(type_flag_t::obj_t);
					h->cl = (length_t)data->size();
				}
				else {
					th = (head_t*)(data->data() + last_push);
					while (th->t == type_flag_t::del_t) {
						th = (head_t*)(data->data() + th->n);
					}
				}
				return json_value(*this, th, key);

				//head_ptr_t th;
				//if (last_push == 0) {
				//	push_head(type_flag_t::obj_t);
				//	h->cl = (length_t)data->size();
				//}
				//else {
				//	th.update(data, last_push);
				//	while (th->t == type_flag_t::del_t) {
				//		th.update(th->n);
				//	}
				//}
				//return json_value(*this, th, key);
			}
		}
		return *this;
	}

	//! get arrray value
	json_value& operator [] (int index) {

		if (index >= 0) {
			// set this head as obj_t
			if (h->t == type_flag_t::pre_t) {
				h->t = type_flag_t::arr_t;
				h->cl = (length_t)data->size();
				push_head(type_flag_t::pre_t);
			}
			else {
				// dont find the head -> add this key
				head_t* th = get_index_head(index);
				if (th == nullptr) {
					push_head(type_flag_t::arr_t);
					h->cl = (length_t)data->size();
					push_head(type_flag_t::pre_t);
				}
				else if (h != th) {
					th->n = (next_t)data->size();
					push_head(type_flag_t::pre_t);
				}
			}
		}
		return *this;
	}

	//! get the next value head
	/*!
		\return	If this is the last value return true else return false
	*/
	bool next(json_stack* iter_stack, length_t begin = 0) {

		while (h->t == type_flag_t::del_t)
			//h = (head_t*)(data->data() + h->n);
			update_head(h->n);
		if ((h->t == type_flag_t::obj_t || h->t == type_flag_t::arr_t) && h->cl) {
			iter_stack->push({ true, (int)((const char*)h - data->data()) });
			//h = (head_t*)(data->data() + h->cl);
			update_head(h->cl);
			return false;
		}
		if (h->n) {
			//h = (head_t*)(data->data() + h->n);
			update_head(h->n);
			return false;
		}
		else {
			if (iter_stack->size()) {
				while (iter_stack->size()) {
					//h = (head_t*)(data->data() + iter_stack->top().off);
					update_head(iter_stack->top().off);
					iter_stack->pop();
					if (h->n) {
						//h = (head_t*)(data->data() + h->n);
						update_head(h->n);
						return false;
					}
				}
				return true;
			}
			else
				//h = (head_t*)(data->data() + begin);
				update_head(begin);
			return true;
		}
	}

	//! get the next value head
	/*!
		\return	If this is the last value return true else return false
	*/
	bool next_brother(length_t begin = 0) {

		while (h->t == type_flag_t::del_t)
			//h = (head_t*)(data->data() + h->n);
			update_head(h->n);
		if (h->n) {
			//h = (head_t*)(data->data() + h->n);
			update_head(h->n);
			return false;
		}
		update_head(begin);
		//h = (head_t*)(data->data() + begin);
		return true;
	}

	//! assign a number
	template<class N>
	void operator = (N num) {

		static_assert(is_arithmetic<N>::value != 0, "Must be arithmetic type");
		if (h) {
			if (h->t == type_flag_t::num_t)
				h->set_num(num);
			else if (h->t == type_flag_t::pre_t) {
				push_num(template_param<N>()) = num;
			}
			else if (h->t == type_flag_t::emp_t) {
				push_head(type_flag_t::num_t);
				push_num(template_param<N>()) = num;
			}
			else {
				h->t = type_flag_t::del_t;
				int pre_n = h->n;
				h->n = (next_t)data->size();
				push_head_from(type_flag_t::num_t, h);
				h->n = pre_n;
				push_num(template_param<N>()) = num;
			}
		}
	}

	//! assign bool
	void operator = (bool num) {

		if (h) {
			if (h->t == type_flag_t::boo_t)
				h->set_num(num);
			else if (h->t == type_flag_t::pre_t) {
				push_num(template_param<bool>()) = num;
				h->cl = 0;
				h->t = type_flag_t::boo_t;
			}
			else if (h->t == type_flag_t::emp_t) {
				push_head(type_flag_t::boo_t);
				push_num(template_param<bool>()) = num;
			}
			else {
				h->t = type_flag_t::del_t;
				int pre_n = h->n;
				h->n = (next_t)data->size();
				push_head_from(type_flag_t::boo_t, h);
				h->n = pre_n;
				push_num(template_param<bool>()) = num;
			}
		}
	}

	//! assign null
	void operator = (nullptr_t null) {

		if (h) {
			if (h->t == type_flag_t::pre_t || h->t == type_flag_t::emp_t) {
				h->t = type_flag_t::nul_t;
			}
			else {
				h->t = type_flag_t::del_t;
				h->n = (next_t)data->size();
				push_head(type_flag_t::nul_t);
			}
		}
	}

	//! assign a string
	void operator = (const char* str) {

		if (h && str) {
			length_t len = (length_t)strlen(str);
			if (h->t == type_flag_t::str_t) {
				if (len <= h->cl) {
					h->cl = len;
					h->set_string(str, len + 1);
				}
				else {
					h->t = type_flag_t::del_t;
					int pre_n = h->n;
					h->n = (next_t)data->size();
					push_head_from(type_flag_t::str_t, h);
					h->cl = len;
					h->n = pre_n;
					push_str(str, len);
				}
			}
			else if (h->t == type_flag_t::pre_t) {
				h->t = type_flag_t::str_t;
				h->cl = len;
				length_t off = data->size();
				push_str(str, len);
			}
			else if (h->t == type_flag_t::emp_t) {
				push_head(type_flag_t::str_t);
				h->cl = len;
				push_str(str, len);
			}
			else if ((h->t == type_flag_t::num_t || h->t == type_flag_t::boo_t) && len < sizeof(number_t)) {
				h->t = type_flag_t::str_t;
				h->cl = len;
				h->set_string(str, len + 1);
			}
			else {
				h->t = type_flag_t::del_t;
				int pre_n = h->n;
				h->n = (next_t)data->size();
				push_head_from(type_flag_t::str_t, h);
				h->cl = len;
				h->n = pre_n;
				push_str(str, len);
			}
		}
	}

	//! get number
	template<class Num>
	operator Num()
	{

		static_assert(is_arithmetic<Num>::value != 0, "Must be arithmetic type");
		return h->get_num(template_param<Num>());
	}

	//! you'd better do not use this stupied way to get string value
	operator string() {

		string res;
		h->get_string(res);
		return res;
	}

	//! an elegant way to get the real type value 
	/*! \brief the traditional way: const char* v = value.get_string() or const char* v = value.get<string>(), urgly
			   this way: const char* v = value;
		\return	implicit convert the json_value to const char*
	*/
	operator const char* () {

		return h->get_string();
	}

	//! compare with string
	/*!
		\tparam N it could be uint8_t,int8_t,uint16_t,int16_t,uint32_t,int32_t,float,double all the number length < 8byte
	*/
	bool operator == (const char* str) {

		if (h->t == type_flag_t::str_t)
			return h->equal(str);
		else
			return false;
	}

	//! compare with number
	/*!
		\tparam N it could be uint8_t,int8_t,uint16_t,int16_t,uint32_t,int32_t,float,double all the number length < 8byte
	*/
	template<class N>
	bool operator == (N num) {

		static_assert(is_arithmetic<N>::value != 0, "Must be arithmetic type");
		if (h->t == type_flag_t::num_t || h->t == type_flag_t::boo_t)
			return h->get_num(template_param<N>()) == num;
		else
			return false;
	}

	//! compare with null
	bool operator == (nullptr_t null) {

		if (h->t == type_flag_t::nul_t)
			return true;
		else
			return false;
	}

	//! erase a value
	/*! \brief	For efficiency, just set the value flag as del_t, so there the memory move will not happen
				when insert a new value the memery may be use again.
	*/
	void erase() {

		if (h) {
			h->t = type_flag_t::del_t;
		}
	}

	//! find a key
	//! \return exist -> true or -> false
	bool find(const char* key) {

		return find_key(key);
	}

	//! find a key
	//! \return exist -> true or -> false
	const json_value& findv(const char* key) {

		if (hash_node* node = find_node(key)) {
			return json_value(*this, head_ptr_t(data, node->value_off), node->value_off);
		}
		return json_value();
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

	bool is_number_int() {

		return h->cl == type_flag_t::num_int_t;
	}

	bool is_number_double() {

		return h->cl == type_flag_t::num_double_t;
	}

	//! get the child size or value size
	/*! \return [],{} -> child size,
				number -> sizeof(number_t) = 8byte
				string -> length(string)
	*/
	size_t size() {

		if (h->t == type_flag_t::arr_t || h->t == type_flag_t::obj_t)
			return count_size();
		else if (h->t == type_flag_t::num_t) {
			return sizeof(number_t);
		}
		return h->cl;
	}

	//! get the key of the value
	/*! \return in [] -> return null
				in {} -> return the key
	*/
	const char* key() {

		if (h->kl)
			return h->get_key();
		else
			return "";
	}

	//! build all the same level data to a map
	/*! \brief If you need to find a value by index in high frequency, this will help you.
			   If you want to know why this api is necessary, you should know about the json_value Memeary model
		\param vh a vector_helper reference
	*/
	void build_vector_helper(vector_helper& vh) {

		if (h && h->t == type_flag_t::arr_t) {
			vh.root = this;
			const char* begin = data->data();
			//// point to the child begin
			head_t* th = (head_t*)(begin + h->cl);
			vh.vec.push_back(h->cl);
			while (th) {
				// if this node was deleted -> jump
				if (th->t != type_flag_t::del_t) {
					vh.vec.push_back(th->n);
				}
				// return the end head
				if (!th->n)
					return;
				// point to the brother head
				th = (head_t*)(begin + th->n);

			}
		}
	}

	//! build all the same level data to a map
	/*! \brief if you need to find a value by key in high frequency, this will help you
		\param mh a map_helper reference
	*/
	void build_map_helper(map_helper& mh) {

		if (h && h->t == type_flag_t::obj_t) {
			mh.root = this;
			const char* begin = data->data();
			//// point to the child begin
			head_t* th = (head_t*)(begin + h->cl);
			while (th) {
				// if this node was deleted -> jump
				if (th->t != type_flag_t::del_t) {
					mh.mapp[th->get_key()] = (length_t)((const char*)th - data->data());
				}
				// return the end head
				if (!th->n)
					return;
				// point to the brother head
				th = (head_t*)(begin + th->n);

			}
		}
	}

	void serialize(string& str) {

		str.resize(0);
		str.reserve(data->size());
		json_stack stack;
		head_t* th = h;

		while (th) {
			if (th->t != type_flag_t::del_t) {
				if (stack.size() && stack.top().is_obj) {
					if (th->kl) {
						str += "\"";
						str += th->get_key();
						str += "\":";
					}
					else {
						str += "\"\":";
					}
				}

				if (th->t == type_flag_t::obj_t) {
					if (th->cl) {
						str += "{";
						stack.push({ true,(int)((const char*)th - data->data()) });
						th = (head_t*)(data->data() + th->cl);
						continue;
					}
					else {
						th = (head_t*)(data->data() + th->n);
						str += "{},";
					}
					//continue;
				}
				else if (th->t == type_flag_t::arr_t) {
					if (th->cl) {
						str += "[";
						stack.push({ false,(int)((const char*)th - data->data()) });
						th = (head_t*)(data->data() + th->cl);
						continue;
					}
					else {
						th = (head_t*)(data->data() + th->n);
						str += "[],";
					}

				}

				if (th->t == type_flag_t::num_t) {
					if (th->cl == type_flag_t::num_int_t) {
						int64_t i64 = th->get_num(template_param<int64_t>());
						parser::serialize(&i64, str);
					}
					else {
						double d64 = th->get_num(template_param<double>());
						parser::serialize(&d64, str);
					}
					str += ',';
				}
				else if (th->t == type_flag_t::str_t) {
					str += "\"";
					str += th->get_string();
					str += "\",";
				}
				else if (th->t == type_flag_t::nul_t) {
					str += "null";
					str += ',';
				}
				else if (th->t == type_flag_t::boo_t) {
					if (th->get_num(template_param<bool>()))
						str += "true";
					else
						str += "false";
					str += ',';
				}
			}
			while (!th->n) {
				str.pop_back();
				if (stack.size()) {
					if (stack.top().is_obj)
						str += "},";
					else
						str += "],";
					th = (head_t*)(data->data() + stack.top().off);
					stack.pop();
				}
				else
					return;
			}
			th = (head_t*)(data->data() + th->n);
		}
	}

	void swap(json_value& jv) {
		data->swap(*jv.data);
	}

	json_value(const json_value& o) {
		data = o.data;
		h = o.h;
		od = data->data();
		hash_table = o.hash_table;
		link_table = o.link_table;
		count = o.count;
		mode_index = o.mode_index;
		last_push = o.last_push;
		ph = o.ph;
	}

	json_value(const json_value& o, const head_ptr_t& h, size_t ph) {
		data = o.data;
		this->h = h;
		od = data->data();
		hash_table = o.hash_table;
		link_table = o.link_table;
		count = o.count;
		mode_index = o.mode_index;
		this->ph = ph;
		if (h->t == obj_t)
			last_push = (int)((const char*)get_end_head() - data->data());
		else
			last_push = o.last_push;
	}

	json_value(json_value& o, const head_ptr_t& th, const char* key) {
		data = o.data;
		h = o.h;
		hash_table = o.hash_table;
		link_table = o.link_table;
		count = o.count;
		mode_index = o.mode_index;
		ph = o.ph;
		if (th == nullptr) {
			ph = get_off();
			//push_head(type_flag_t::obj_t);
			//h->cl = (length_t)data->size();
			push_head_nofind(type_flag_t::pre_t, key, (length_t)strlen(key));
		}
		else {
			th->n = (next_t)data->size();
			push_head_nofind(type_flag_t::pre_t, key, (length_t)strlen(key));
		}
		o.last_push = get_off();
	}

	json_value(json_value& o, head_t* th, const char* key) {
		data = o.data;
		h = o.h;
		hash_table = o.hash_table;
		link_table = o.link_table;
		count = o.count;
		mode_index = o.mode_index;
		ph = o.ph;
		if (th == nullptr) {
			ph = get_off();
			//push_head(type_flag_t::obj_t);
			//h->cl = (length_t)data->size();
			push_head_nofind(type_flag_t::pre_t, key, (length_t)strlen(key));
		}
		else {
			th->n = (next_t)data->size();
			push_head_nofind(type_flag_t::pre_t, key, (length_t)strlen(key));
		}
		o.last_push = get_off();
	}

	void add_node(const char* key, size_t value_off) {
		if (*count + 1 > prime_list[*mode_index]) {
			(*mode_index)++;
			hash_table_t t_hash_table(move(*hash_table));
			link_table_t t_link_table(move(*link_table));
			hash_table->resize(prime_list[*mode_index]);
			*count = 0;
			for (auto& tn : t_hash_table) {
				head_t* th = (head_t*)(data->data() + tn.value_off);
				if (tn.value_off && th->t != type_flag_t::del_t) {
					head_t* th = (head_t*)(data->data() + tn.value_off);
					hash<no_copy_string> h;
					size_t hk = h(th->get_key());
					add_node(hk, tn.value_off, tn.parent);
					size_t next = tn.next;
					while (next) {
						auto& ln = t_link_table[next - 1];
						head_t* th = (head_t*)(data->data() + ln.value_off);
						if (th->t != type_flag_t::del_t) {
							hash<no_copy_string> h;
							size_t hk = h(th->get_key());
							add_node(hk, ln.value_off, ln.parent);
						}
						next = ln.next;
					}
				}
			}
		}
		hash<no_copy_string> h;
		size_t hk = h(key);
		if (hash_node* node = find_node(hk, key)) {
			node->value_off = value_off;
			return;
		}
		add_node(hk, value_off, ph);
	}

	void add_node_nofind(const char* key, size_t value_off) {
		if (*count + 1 > prime_list[*mode_index]) {
			(*mode_index)++;
			hash_table_t t_hash_table(move(*hash_table));
			link_table_t t_link_table(move(*link_table));
			hash_table->resize(prime_list[*mode_index]);
			*count = 0;
			for (auto& tn : t_hash_table) {
				head_t* th = (head_t*)(data->data() + tn.value_off);
				if (tn.value_off && th->t != type_flag_t::del_t) {
					hash<no_copy_string> h;
					size_t hk = h(th->get_key());
					add_node(hk, tn.value_off, tn.parent);
					size_t next = tn.next;
					while (next) {
						auto& ln = t_link_table[next - 1];
						head_t* th = (head_t*)(data->data() + ln.value_off);
						if (th->t != type_flag_t::del_t) {
							hash<no_copy_string> h;
							size_t hk = h(th->get_key());
							add_node(hk, ln.value_off, ln.parent);
						}
						next = ln.next;
					}
				}
			}
		}
		hash<no_copy_string> h;
		size_t hk = h(key);
		add_node(hk, value_off, ph);
	}

	void add_node(size_t key, size_t value_off, size_t ph) {
		size_t index = key % prime_list[*mode_index];
		hash_node& node = (*hash_table)[index];
		if (node.value_off == 0) {
			node.value_off = value_off;
			node.parent = ph;
		}
		else {
			link_table->push_back({ node.next, (uint32_t)ph, (uint32_t)value_off });
			node.next = link_table->size();
		}
		(*count)++;
	}

	hash_node* find_node(const char* key) {
		hash<no_copy_string> h;
		size_t hk = h(key);
		return find_node(hk, key);
	}

	hash_node* find_node(size_t hk, const char* k) {
		size_t index = hk % prime_list[*mode_index];
		hash_node& node = (*hash_table)[index];
		head_t* th = (head_t*)(data->data() + node.value_off);
		if (ph == node.parent && th->keycmp(k) && th->t != type_flag_t::del_t) {
			return &node;
		}
		size_t next = node.next;
		while (next) {
			hash_node& next_node = (*link_table)[next - 1];
			th = (head_t*)(data->data() + next_node.value_off);
			if (ph != next_node.parent || !th->keycmp(k) || th->t == type_flag_t::del_t) {
				next = next_node.next;
				continue;
			}
			return &next_node;
		}
		return nullptr;
	}

private:
	// delete the copy structor and operator, avoid freshman to do some stupid things
	// if you really really want to copy a entity, please use copy_from
	//json_value(const json_value &) = delete;
	json_value& operator = (const json_value&) = delete;

	head_t* goto_next_usable_head() {
		if (h) {
			const char* begin = data->data();
			// point to the child begin
			head_t* th = h;
			while (th) {
				// if this node was deleted -> jump
				if (th->t != type_flag_t::del_t) {
					h = th;
					return th;
				}
				// return the end head
				if (!th->n)
					return th;
				// point to the brother head
				th = (head_t*)(begin + th->n);
			}
			return th;
		}
		return nullptr;
	}

	length_t count_size() {
		if (h && h->cl) {
			// point to the child begin
			head_t* th = (head_t*)(data->data() + h->cl);
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
				th = (head_t*)(data->data() + th->n);

			}
			return i;
		}
		return 0;
	}
	head_t* get_index_head(length_t index) {
		if (h && h->t == type_flag_t::arr_t) {
			const char* begin = data->data();
			// point to the child begin

			head_t* th = (head_t*)(begin + h->cl);
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
				th = (head_t*)(begin + th->n);

			}
			return th;
		}
		return nullptr;
	}

	head_t* get_key_head(const char* key, bool& is_find) {
		if (h && h->t == type_flag_t::obj_t) {
			const char* begin = data->data();
			// point to the child begin
			head_t* th = (head_t*)(begin + h->cl);
			while (th) {
				// if this node was deleted -> jump
				if (th->t != type_flag_t::del_t) {
					// if find the key -> reset the head
					if (th->keycmp(key)) {
						//h = th;
						is_find = true;
						return th;
					}
				}
				// return the end head
				if (!th->n)
					return th;
				// point to the brother head
				th = (head_t*)(begin + th->n);
			}
			return th;
		}
		return nullptr;
	}

	head_t* get_end_head() {
		if (h && h->t == type_flag_t::obj_t) {
			const char* begin = data->data();
			// point to the child begin
			head_t* th = (head_t*)(begin + h->cl);
			while (th->n) {
				// point to the brother head
				th = (head_t*)(begin + th->n);
			}
			return th;
		}
		return nullptr;
	}

	bool find_key(const char* key) {
		if (h && h->t == type_flag_t::obj_t) {
			const char* begin = data->data();
			// point to the child begin
			head_t* th = (head_t*)(begin + h->cl);
			while (th) {
				// if this node was deleted -> jump
				if (th->t != type_flag_t::del_t) {
					// if find the key -> reset the head
					if (th->keycmp(key)) {
						return true;
					}
				}
				// return the end head
				if (!th->n)
					return false;
				// point to the brother head
				th = (head_t*)(begin + th->n);
			}
			return false;
		}
		return false;
	}

	bool find_key(const char* key, size_t off) {
		head_t* th = (head_t*)(data->data() + off);
		if (th->keycmp(key))
			return true;
		return false;
	}

protected:
	inline void update_head(int off) {
		h.update(off);
	}

	void pre_allocate(size_t base_size) {
		data->reserve(base_size + base_size / 3);
		data->resize(0);
	}

	json_value& to_off(int off) {
		h.update(off);
		return *this;
	}

	inline void set_next() {
		h->n = (next_t)data->size();
	}

	inline void update_cl() {
		h->cl = (length_t)data->size();
	}

	inline void update_cl(length_t length) {
		h->cl = length;
	}

	inline void update_cl0() {
		h->cl = 0;
	}

	inline flag_t get_flag() {
		return h->t;
	}

	inline length_t get_off() {
		return h.offset;
	}

	void push_head(flag_t t) {
		//add head
		//update_head(data->size());
		h.refresh();
		data->resize(data->size() + head_t::head_size());
		h->t = t;
	}

	void push_head(flag_t t, const char* key, length_t kl) {
		//add head
		//update_head(data->size());
		h.refresh();
		data->resize(data->size() + head_t::head_size());
		//add key end with '\0'
		push_str(key, kl);

		h->t = t;

		if (kl < 255)
			h->kl = kl;
		else
			h->kl = 255;

		add_node(string(key, kl).c_str(), h.offset);
	}

	void push_head_nofind(flag_t t, const char* key, length_t kl) {
		//add head
		//update_head(data->size());
		h.refresh();
		data->resize(data->size() + head_t::head_size());
		//add key end with '\0'
		push_str(key, kl);

		h->t = t;

		if (kl < 255)
			h->kl = kl;
		else
			h->kl = 255;

		add_node_nofind(key, h.offset);
	}

	inline std::string& get_data() {
		return *((std::string*)data);
	}

	void set_flag(char f) {
		h->t = f;
	}

	void set_ph() {
		ph = get_off();
	}

	void push_head_from(flag_t t, head_ptr_t& from) {
		//add head
		//update_head(data->size());
		length_t off = length_t(data->size());
		data->resize(data->size() + head_t::head_size());
		//add key end with '\0'
		key_t kl = from->kl;
		if (kl)
			push_str(from->get_key(), kl);
		//cur head
		update_head(off);
		//old_head
		h->t = t;
		h->kl = kl;

		add_node(h->get_key(), off);
	}

	void grow(size_t size) {
		length_t off = length_t(data->size());
		data->resize(data->size() + size);
		update_head(off);
	}


	inline void push_str(const char* str, length_t len) {
		data->append(str, len);
		*data += '\0';
	}

	template<class N>
	inline N& push_num(const template_param<N>& num) {
		h->t = type_flag_t::num_t;
		data->resize(data->size() + sizeof(number_t));
		set_num_cl(num);
		return *(N*)h->get_val();
	}

	template<class N>
	inline void set_num_cl(const template_param<N>&) {
		h->cl = type_flag_t::num_int_t;
	}

	inline void set_num_cl(const template_param<double>&) {
		h->cl = type_flag_t::num_double_t;
	}

	inline void set_num_cl(const template_param<float>&) {
		h->cl = type_flag_t::num_double_t;
	}

	void root_copy(json_value& jv) {
		data = jv.data;
		h = jv.h;
		od = jv.od;
	}

private:
	data_t* data;
	//head_t* h;
	head_ptr_t h;
	const char* od;
	size_t ph;

	size_t last_push;

	hash_table_t* hash_table;
	link_table_t* link_table;
	link_table_t* other_table;
	uint32_t* mode_index;
	uint32_t* count;
};

//! a Non - recursive based parser
template<class DATA_ALLOC = d_alloc_t, class HASH_ALLOC = h_alloc_t>
class dynamic_json_proxy :public json_value<DATA_ALLOC, HASH_ALLOC> {
public:
	typedef json_value<DATA_ALLOC, HASH_ALLOC> json_value_t;
	typedef uint32_t length_t;

	dynamic_json_proxy() : json_value_t() {
	}
	dynamic_json_proxy(const char* key) : json_value_t(key) {
	}
	~dynamic_json_proxy() {
		json_value_t::release();
	}
private:
	bool parse_string(json_stream& js) {
		parser::get_next(js);
		json_value_t::set_flag(json_value_t::type_flag_t::str_t);
		int head_off = json_value_t::get_off();
		size_t end = json_value_t::get_data().size();
		if (!parser::parse_str(json_value_t::get_data(), js)) {
			ERROR_RETURT(js);
		}
		json_value_t::update_head(head_off);
		json_value_t::update_cl(length_t(json_value_t::get_data().size() - end));
		json_value_t::get_data() += '\0';
		return true;
	}

	bool parse_number(json_stream& js) {
		if (char ch = *js.begin) {
			if (ch == 'f' || ch == 't') {
				if (!parser::parse_bool(json_value_t::push_num(template_param<bool>()), js)) {
					ERROR_RETURT(js);
				}
				json_value_t::set_flag(json_value_t::type_flag_t::boo_t);
			}
			else if (ch == 'n') {
				if (!parser::is_null(js)) {
					ERROR_RETURT(js);
				}
				json_value_t::set_flag(json_value_t::type_flag_t::nul_t);
			}
			else {
				char res = parser::is_double(js);
				if (res == -1) {
					if (!parser::unserialize(&json_value_t::push_num(template_param<double>()), js)) {
						ERROR_RETURT(js);
					}
				}
				else if (res == 1) {
					if (!parser::unserialize(&json_value_t::push_num(template_param<uint64_t>()), js)) {
						ERROR_RETURT(js);
					}
				}
				else {
					ERROR_RETURT(js);
				}
			}
		}
		return true;
	}


	//! Non recursive implementation, so there is no limit on the depth, it is up on your memory size
	/*! \brief 	obj:{ -> "key" -> : -> value -> }
				arr:[ -> value -> ]
		\return a reference of json_value
	*/
	bool parse(json_stream& js) {
		json_stack stack;
		parser::skip_space(js);


		if (*js.begin == parser::json_key_symbol::str) {
			json_value_t::push_head(json_value_t::type_flag_t::pre_t);
			if (!parse_string(js))
				return false;
			parser::skip_space(js);
			if (parser::get_cur_and_next(js) != '\0') {
				ERROR_RETURT(js);
			}
			return true;
		}
		else if (*js.begin == parser::json_key_symbol::object_begin || *js.begin == parser::json_key_symbol::array_begin) {
			while (char ch = parser::get_cur_and_next(js)) {
				//step 1: check start tokens , [ {
				//--------------------------------------------------------
				parser::skip_space(js);
				// , end and begin token
				if (ch == parser::json_key_symbol::next_key_value) {
					json_value_t::set_next();
				}
				else if (ch == parser::json_key_symbol::object_begin) {
					if (json_value_t::get_flag() == json_value_t::type_flag_t::pre_t) {
						json_value_t::set_flag(json_value_t::type_flag_t::obj_t);
						json_value_t::update_cl();
						//set_ph();
					}
					else {
						json_value_t::set_ph();
						json_value_t::push_head(json_value_t::type_flag_t::obj_t);
						json_value_t::update_cl();
					}
					stack.push({ true,(int)json_value_t::get_off() });
				}
				else if (ch == parser::json_key_symbol::array_begin) {
					//[], empty array
					if (*js.begin == parser::json_key_symbol::array_end) {
						parser::get_next(js);
						parser::skip_space(js);
						json_value_t::set_flag(json_value_t::type_flag_t::arr_t);
						json_value_t::update_cl0();
						continue;
					}
					//[x,x,x] and [],[],[]
					if (json_value_t::get_flag() == json_value_t::type_flag_t::pre_t) {
						json_value_t::set_flag(json_value_t::type_flag_t::arr_t);
						json_value_t::update_cl();
					}
					else {
						json_value_t::push_head(json_value_t::type_flag_t::arr_t);
						json_value_t::update_cl();
					}
					stack.push({ false,(int)json_value_t::get_off() });
				}
				else if (ch == parser::json_key_symbol::object_end) {
					if (stack.size() > 0 && stack.top().is_obj) {
						json_value_t::update_head(stack.top().off);
						stack.pop();
					}
					else {
						ERROR_RETURT(js);
					}
					parser::skip_space(js);
					continue;
				}
				else if (ch == parser::json_key_symbol::array_end) {
					if (stack.size() > 0 && !stack.top().is_obj) {
						json_value_t::update_head(stack.top().off);
						stack.pop();
					}
					else {
						ERROR_RETURT(js);
					}
					parser::skip_space(js);
					continue;
				}
				if (stack.size() == 0) {
					ERROR_RETURT(js);
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
							json_value_t::push_head(json_value_t::type_flag_t::pre_t, b, (length_t)parser::skip_str(js));

							//check key_value separator
							parser::skip_space(js);
							if (parser::get_cur_and_next(js) != parser::json_key_symbol::key_value_separator) {
								ERROR_RETURT(js);
							}
						}
						//if no value pop stack
						else if (ch == parser::json_key_symbol::object_end) {
							json_value_t::update_cl0();
							stack.pop();
							continue;
						}
						else {
							//error format
							ERROR_RETURT(js);
						}
					}
					else {
						//error format
						ERROR_RETURT(js);
					}
				}
				else {
					json_value_t::push_head(json_value_t::type_flag_t::pre_t);
				}

				//step 3: parse value
				//-----------------------------------------------------------
				parser::skip_space(js);
				if (char ch = *js.begin) {
					if (ch == parser::json_key_symbol::str) {
						if (!parse_string(js))
							return false;
					}
					else if (ch == parser::json_key_symbol::object_begin) {
						continue;
					}
					else if (ch == parser::json_key_symbol::array_begin) {
						continue;
					}
					else {
						if (!parse_number(js))
							return false;
					}
				}
				parser::skip_space(js);

				//step 4: check end tokens
				//-----------------------------------------------------------
				//check end token only } ] , is allowed
				if (*js.begin == parser::json_key_symbol::next_key_value || *js.begin == parser::json_key_symbol::object_end || *js.begin == parser::json_key_symbol::array_end) {
					continue;
				}

				ERROR_RETURT(js);
				//-----------------------------------------------------------
			}

			if (stack.size() == 0)
				return true;
		}
		else {
			json_value_t::push_head(json_value_t::type_flag_t::pre_t);
			if (!parse_number(js))
				return false;
			parser::skip_space(js);
			if (parser::get_cur_and_next(js) != '\0') {
				ERROR_RETURT(js);
			}
			return true;
		}
		return false;
	}
public:
	void copy_from(json_value_t& jv) {
		root_copy(jv);
	}

	//! get a value in object by key
	/*! \param key
		\return a reference of json_value
	*/
	json_value_t operator [] (const char* key) {
		return json_value_t::operator[](key);
	}

	//! get a value in array by index
	/*! \param index
		\return a reference of json_value
	*/
	json_value_t& operator [] (int index) {
		return json_value_t::operator[](index);
	}

	//! assign a number
	template<class N>
	void operator = (N num) {
		return json_value_t::operator=(num);
	}

	//! assign a string
	void operator = (const char* str) {
		return json_value_t::operator=(str);
	}

	//! assign a string
	void operator = (bool num) {
		return json_value_t::operator=(num);
	}

	//! assign null
	void operator = (nullptr_t null) {
		return json_value_t::operator=(null);
	}

	//json_value& next(traverse_helper& helper) {
	//	init();
	//	return json_value::next(helper);
	//}

	void serialize(string& str) {
		return json_value_t::serialize(str);
	}

	//! unserialize a string with length
	/*! \brief In this API, the value memery will pre_allocated once time, this is benificial to parse a big json with a high memery
			   allocation efficiency, algorithm: size = size + size / 3
		\param js json format string
		\param size the length of the jsonbuffer
		\param option ASSERT|UNESCAPE|UNESCAPE_UNICODE, when there are multiple options, you can combat them by OR operation
		\return the lenght of
	*/
	size_t unserialize(const char* json, size_t size, char option = 0) {
		json_value_t::pre_allocate(size);
		const char* begin = json;
		json_stream js{ begin,begin + size,option };
		if (parse(js))
			return js.begin - json;
		return 0;
	}

	//! unserialize a string with end flag '\0'.
	/*! \param js json format string
		\param option ASSERT|UNESCAPE|UNESCAPE_UNICODE, when there are multiple options, you can combat them by OR operation
		\return the lenght of
	*/
	size_t unserialize(const char* json, char option = 0) {
		json_value_t::pre_allocate(0);
		const char* begin = json;
		json_stream js{ begin,nullptr,option };
		if (parse(js))
			return js.begin - json;
		return 0;
	}

	//! unserialize a string.
	/*! \param js json format string
		\param option ASSERT|UNESCAPE|UNESCAPE_UNICODE, when there are multiple options, you can combat them by OR operation
		\return the lenght of
	*/
	size_t unserialize(const string& js, char option = 0) {
		return unserialize(js.data(), js.size(), option);
	}

};

typedef dynamic_json_proxy<> dynamic_json;
typedef dynamic_json_proxy<shm_data_alloc_t, shm_hash_alloc_t> shm_dynamic_json;