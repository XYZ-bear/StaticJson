#pragma once
#include "doctest.h"
#include "static_def.h"
#include "dynamic.h"
#include "perf.h"

#define TEST_TITLE(c) cout<<endl<<"<perf desc>"<<c<<endl

TEST_CASE("perf insert str key test") {
	TEST_TITLE("perf insert str key test");
	dynamic_json dj;
	size_t repeat = 100000;
	PERF(dynamic, repeat){
		dj.insert(to_string(_idx).c_str(), _idx);
	}

	unordered_map<string, int> mp;
	PERF(unorderd_map, repeat){
		mp.insert({to_string(_idx), _idx});
	}

	map<string, int> mmp;
	PERF(map, repeat){
		mmp.insert({to_string(_idx), _idx});
	}

    CHECK((perf_test_dynamic.cost < perf_test_unorderd_map.cost));
}

TEST_CASE("perf insert int key test") {
	TEST_TITLE("perf insert int key test");
	dynamic_json dj;
	size_t repeat = 100000;
	PERF(dynamic, repeat){
		dj.insert(_idx, _idx);
	}

	unordered_map<int, int> mp;
	PERF(unorderd_map, repeat){
		mp.insert({_idx, _idx});
	}

   CHECK((perf_test_dynamic.cost < perf_test_unorderd_map.cost));
}

TEST_CASE("perf insert str kv test") {
	TEST_TITLE("perf insert str kv test");
	dynamic_json dj;
	size_t repeat = 100000;
	PERF(dynamic, repeat){
        string s=to_string(_idx);
		dj.insert(s.c_str(), s.c_str());
	}

	unordered_map<string, string> mp;
	PERF(unorderd_map, repeat){
		string s=to_string(_idx);
		mp.insert({s, s});
	}

    CHECK((perf_test_dynamic.cost < perf_test_unorderd_map.cost));
}

TEST_CASE("perf find str k test") {
	TEST_TITLE("perf find str k test");
	dynamic_json dj;
	size_t repeat = 100000;
    for(int i=0;i<repeat;i++){
        string s=to_string(i);
        dj.insert(s.c_str(), s.c_str());
    }

	PERF(dynamic, repeat){
		string s=to_string(_idx);
		dj.find(s.c_str());
	}

	unordered_map<string, string> mp;
    for(int i=0;i<repeat;i++){
        string s=to_string(i);
        mp.insert({s, s});
    }
	PERF(unorderd_map, repeat){
        string s=to_string(_idx);
		mp.find(s) == mp.end();
	}

	map<string, string> mmp;
    for(int i=0;i<repeat;i++){
        string s=to_string(i);
        mmp.insert({s, s});
    }
	PERF(map, repeat){
        string s=to_string(_idx);
		mmp.find(s) == mmp.end();
	}

    //CHECK((perf_test_find_str.cost < perf_test_unorderd_map_find_str.cost));
}

TEST_CASE("perf find int k test") {
	TEST_TITLE("perf find int k test");
	dynamic_json dj;
	size_t repeat = 100000;
    for(int i=0;i<repeat;i++){
        dj.insert(i, i);
    }

	PERF(dynamic, repeat){
		dj.find(_idx);
	}

	unordered_map<int, int> mp;
    for(int i=0;i<repeat;i++){
        mp.insert({i, i});
    }
	PERF(unorderd_map, repeat){
		mp.find(_idx) == mp.end();
	}

   // CHECK((perf_test_find_int.cost < perf_test_unorderd_map_find_int.cost));
}

TEST_CASE("perf erase str k test") {
	TEST_TITLE("perf erase str k test");
	dynamic_json dj;
	size_t repeat = 100000;
    for(int i=0;i<repeat;i++){
        string s=to_string(i);
        dj.insert(s.c_str(), s.c_str());
    }

	PERF(dynamic, repeat){
        string s=to_string(_idx);
		dj.erase(s.c_str());
	}

	unordered_map<string, string> mp;
    for(int i=0;i<repeat;i++){
        string s=to_string(i);
        mp.insert({s, s});
    }
	PERF(unorderd_map, repeat){
		string s=to_string(_idx);
		mp.erase(s);
	}

    CHECK((perf_test_dynamic.cost < perf_test_unorderd_map.cost));
}

TEST_CASE("perf erase int k test") {
	TEST_TITLE("perf erase int k test");
	dynamic_json dj;
	size_t repeat = 100000;
    for(int i=0;i<repeat;i++){
        dj.insert(i, i);
    }

	PERF(dynamic, repeat){
		dj.erase(_idx);
	}

	unordered_map<int, int> mp;
    for(int i=0;i<repeat;i++){
        mp.insert({i, i});
    }
	PERF(unorderd_map, repeat){
		mp.erase(_idx);
	}

	map<int, int> mmp;
    for(int i=0;i<repeat;i++){
        mmp.insert({i, i});
    }
	PERF(map, repeat){
		mmp.erase(_idx);
	}

   CHECK((perf_test_dynamic.cost < perf_test_unorderd_map.cost));
}

TEST_CASE("perf random insert_erase test") {
	TEST_TITLE("perf random insert_erase test");
	
	size_t repeat = 100000;
	srand((unsigned)time(NULL)); 

	dynamic_json dj;
	PERF(dynamic, repeat){
		size_t r = rand() % 100;
		if(r < 50)
			dj.insert(to_string(_idx).c_str(), _idx);
		else
			dj.erase(to_string(rand() % (_idx + 1)).c_str());
	}

	unordered_map<string, int> mp;
	PERF(unorderd_map, repeat){
		size_t r = rand() % 100;
		if(r < 50)
			mp.insert({to_string(_idx), _idx});
		else
			mp.erase(to_string(rand() % (_idx + 1)));
	}
}

TEST_CASE("perf random insert_erase 2 test") {
	TEST_TITLE("perf random insert_erase 2 test");
	
	size_t repeat = 100000;
	srand((unsigned)time(NULL)); 

	dynamic_json dj;
	PERF(dynamic, repeat){
		size_t r = rand() % 100;
		if(r < 20)
			dj.insert(to_string(_idx).c_str(), _idx);
		else if(r>=20 && r<40){
			dj.insert(_idx, _idx);
		}
		else if(r>=40 && r<60){
			dj.erase(rand() % (_idx + 1));
		}
		else if(r>=60 && r<80){
			dj.erase(to_string(rand() % (_idx + 1)).c_str());
		}
		else if(r>=80 && r<90){
			dj.find(rand() % (_idx + 1));
		}
		else
			dj.find(to_string(rand() % (_idx + 1)).c_str());
	}

	unordered_map<string, int> mp;
	unordered_map<int, int> mp2;
	PERF(unorderd_map, repeat){
		size_t r = rand() % 100;
		if(r < 50)
			mp.insert({to_string(_idx), _idx});
		else if(r>=20 && r<40){
			mp2.insert({_idx, _idx});
		}
		else if(r>=40 && r<60){
			mp2.erase(rand() % (_idx + 1));
		}
		else if(r>=60 && r<80){
			mp.erase(to_string(rand() % (_idx + 1)));
		}
		else if(r>=80 && r<90){
			mp2.find(rand() % (_idx + 1));
		}
		else
			mp.find(to_string(rand() % (_idx + 1)));
	}
}

TEST_CASE("perf recursion insert test") {
	TEST_TITLE("perf recursion insert test");
	
	size_t repeat = 10000;
	srand((unsigned)time(NULL)); 

	dynamic_json dj;
	PERF(dynamic, repeat){
		auto p=dj[rand() % repeat];
		for(int i=0;i<100;i++){
			p.insert(rand() % repeat, i);
		}
	}

	typedef unordered_map<int, int> uodbase;
	typedef unordered_map<int, uodbase*> uodt1;
	uodt1 *ud = new uodt1();
	PERF(unordered_map, repeat){
		auto kk = rand() % repeat;
		auto res = ud->find(kk);
		if(res == ud->end()){
			uodbase *t = new uodbase();
			for(int i=0;i<100;i++){
				t->insert({rand() % repeat, i});
			}
			ud->insert({kk, t});
		}
		else{
			for(int i=0;i<100;i++){
				(*res).second->insert({rand() % repeat, i});
			}
		}
	}
}