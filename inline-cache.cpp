#include <vector>
#include <map>
#include <string>
#include <time.h>

using namespace std;
class JsClass;
class Property;


JsClass* maps;
int map_count = 0;


class Property{
public:
    // if index equals -1, which means it's not a property in this map.
    // we need to transit the class into another.
    Property(int i) : index(i), transit(nullptr) {}
    Property(JsClass* map): index(-1), transit(map) {}
    Property() : index(-1), transit(nullptr) {}

    int index;
    JsClass* transit;
};

class JsClass{
private:
    // only for generate a new map.
    JsClass(int i, JsClass* before) : index(i) {
        for(auto key : before->keys){
            keys.push_back(key);
            Property pro = before->descriptors[key];
            if(pro.index != -1) descriptors[key] = pro;
        }
    }

public:
    JsClass() : index(0) {
        map_count ++;
    } // only for first root map.


    JsClass* transfer(string key){
        maps[map_count] = JsClass(map_count, this);
        maps[map_count].descriptors[key] = Property(keys.size());
        maps[map_count].keys.push_back(key);
        
        descriptors[key] = Property(&maps[map_count]);
        
        return &maps[map_count ++];
    }

    int findIndex(string key){
        if(descriptors.count(key) == 0) return -1;
        return descriptors[key].index;
    }


    map<string, Property> descriptors;
    vector<string> keys;
    int index;
};

class JsObject{
public:
    JsObject(JsClass* m) : map(m) {
        properties.resize(map->keys.size());
    }

    JsObject() : map(&maps[0]) {
        properties.resize(map->keys.size());
    }

    int load(string k){
        int index = map->findIndex(k);
        if(index == -1) abort();
        return properties[index];
    }

    void store(string k, int v){
        int index = map->findIndex(k);
        if(index == -1) {
            // transit to another map.
            map = map->transfer(k);
            index = map->findIndex(k);
            properties.resize(map->keys.size());
        }
        properties[index] = v;
    }

    JsClass* map;
    vector<int> properties;
};

class LoadIC {
public:
    LoadIC(string k) : key(k), map(nullptr), index(-1) {}

    int ic_load(JsObject* o, string k){
        if(o == nullptr) abort();
        if(map == o->map){
            return o->properties[index];
            
        }
        else {
            map = o->map;
            index = o->map->findIndex(k);
            if(index == -1) abort();
            return o->load(k);
        }
    }

    string key;
    JsClass* map;
    int index;
};

class StoreIC {
public:
    StoreIC(string k) : key(k), map(nullptr), index(-1) {}

    void ic_store(JsObject* o, string k, int value){
        if(o == nullptr) abort();
        if(map == o->map){
            o->properties[index] = value;
        }
        else {
            map = o->map;
            index = o->map->findIndex(k);
            if(index == -1) abort();
            o->properties[index] = value;
        }
    }

    string key;
    JsClass* map;
    int index;
};


int main(){
    maps = (JsClass*) malloc(sizeof(JsClass) * 100000);
    maps[0] = JsClass();

    JsObject a = JsObject();
    a.store("a", 0);
    clock_t start, end;

    LoadIC a_load = LoadIC("a");
    StoreIC a_store = StoreIC("a");
    start = clock();
    for(int i = 0; i < 10000000; i ++){
        a_store.ic_store(&a, "a", i);
        if(a_load.ic_load(&a, "a") != i){
            printf("error!\n");
            abort();
        }
    }
    end = clock();
    printf("IC: %f seconds\n",(double)(end-start)/CLOCKS_PER_SEC);

    start = clock();
    for(int i = 0; i < 10000000; i ++){
        a.store("a", i);
        if(a.load("a") != i){
            printf("error!\n");
            abort();
        }
    }
    end = clock();
    printf("No IC: %f seconds\n",(double)(end-start)/CLOCKS_PER_SEC);

    return 0;
}