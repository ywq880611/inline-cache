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

    int load(string k){
        int index = map->findIndex(k);
        if(index == -1) abort();
        return properties[index];
    }

    void store(string k, int v){
        int index = map->findIndex(k);
        if(index == -1) abort();
        properties[index] = v;
    }

    JsClass* map;
    vector<int> properties;
};


int main(){
    maps = (JsClass*) malloc(sizeof(JsClass) * 100000);
    maps[0] = JsClass();
    JsClass* A = maps[0].transfer("a");
    JsObject a = JsObject(A);

    clock_t start = clock();
    for(int i = 0; i < 10000000; i ++){
        a.store("a", i);
        if(a.load("a") != i){
            printf("error!\n");
            abort();
        }
    }
    clock_t end = clock();
    printf("%f seconds\n",(double)(end-start)/CLOCKS_PER_SEC);

    return 0;
}