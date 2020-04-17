#include "map.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
/** The initial size of the map key-value array */
#define INITIAL_SIZE 10
/** The factor by which to expand the key-value array when needed */
#define EXPAND_FACTOR 2
/** The pointer points on NULL argument or if the element was not found**/
#define ELEMENT_NOT_FOUND -1

typedef struct KeyValue {
    char* key;
    char* value;
} *keyValue;

struct Map_t {
    keyValue* elements;
    int size;
    int max_size;
    int iterator;
};

static char* copyString(const char* str) {
    char* newStr = malloc(strlen(str) + 1);
    if (newStr == NULL){
        return NULL;
    }
    return strcpy(newStr, str);
}

static MapResult addOrDestroy(Map map,keyValue element) {
    char* tmp_key = copyString(element->key);
    char* tmp_value = copyString(element->value);
    MapResult result = mapPut(map, tmp_key,tmp_value);
    if (result == MAP_OUT_OF_MEMORY) {
        mapDestroy(map);
    }
    return result;
}

static MapResult addAllOrDestroy(Map map, Map toAdd) {
for (int i = 0; i < toAdd->size; ++i) {
    keyValue element=toAdd->elements[i];
    if (addOrDestroy(map,element ) == MAP_OUT_OF_MEMORY) {
        return MAP_OUT_OF_MEMORY;
    }
}
    return MAP_SUCCESS;
}

static int mapFind(Map map, const char* key) {
    for (int i = 0; i < map->size; i++) {
        keyValue element = map->elements[i];
        if (strcmp(element->key, key) == 0) {
            return i;
    }
}
 return ELEMENT_NOT_FOUND;
}

static MapResult expand(Map map) {
    int new_size = EXPAND_FACTOR * map->max_size;
    keyValue* newElements = realloc(map->elements, new_size * sizeof(keyValue));
    if (newElements == NULL) {
        return MAP_OUT_OF_MEMORY;
    }
    map->elements = newElements;
    map->max_size = new_size;
    return MAP_SUCCESS;
}

static MapResult createKeyValue(Map map, const char* key, const char* data) {
    // we use this function inside another one that checks for null arguments
    assert(map != NULL && data != NULL && key != NULL);
    int index = map->size;
    map->elements[index] = malloc(sizeof(map->elements[index])); // allocates space for the keyValue obj
    map->elements[index]->key = copyString(key);
    map->elements[index]->value = copyString(data);
    map->size++;
    return MAP_SUCCESS;
}

Map mapCreate() {
    Map map = malloc(sizeof(*map));
    if (map == NULL) {
        return NULL;
    }
    map->elements = malloc(INITIAL_SIZE * sizeof(keyValue));
    if (map->elements == NULL) {
        free(map);
        return NULL;
    }
    map->size = 0;
    map->max_size = INITIAL_SIZE;
    map->iterator = 0;
    return map;
}

void mapDestroy(Map map){
    if(mapClear(map) != MAP_NULL_ARGUMENT){
        free(map);
    }  
}

Map mapCopy(Map map) {
    if (map == NULL) {
        return NULL;
    }
    Map newMap = mapCreate();
    if (newMap == NULL) {
        return NULL;
    }
    if (addAllOrDestroy(newMap, map)== MAP_OUT_OF_MEMORY) {
        return NULL;
    }
    newMap->iterator = map->iterator;
    return newMap;
}

int mapGetSize(Map map) {
    if(map == NULL){
        return ELEMENT_NOT_FOUND;
    }
    return map->size;
}

bool mapContains(Map map, const char* key) {
    if (map==NULL || key==NULL) {
        return false;
    }
    return mapFind(map,key)!=ELEMENT_NOT_FOUND;
}

MapResult mapPut(Map map, const char* key, const char* data) {
    if (map == NULL || key == NULL || data == NULL) {
        return MAP_NULL_ARGUMENT;
    }
    int index = mapFind(map,key);
    if (index != ELEMENT_NOT_FOUND) {
        map->elements[index]->value = copyString(data); //assigning the requested data to the value corresponds to the key
        return MAP_SUCCESS;
    }
    if (map->size == map->max_size) {
        if (expand(map) == MAP_OUT_OF_MEMORY) {
            return MAP_OUT_OF_MEMORY;
        }
    }
    return createKeyValue(map, key ,data); //creates a new key-value
}

char* mapGet(Map map, const char* key){
    if(map == NULL || key == NULL){
        return NULL;
    }
    int index=mapFind(map,key);
    if(index==ELEMENT_NOT_FOUND){
        return NULL;
    }
    keyValue element=map->elements[index];
    return element->value;
}    

MapResult mapRemove(Map map, const char* key){
    if(map == NULL || key == NULL){
        return MAP_NULL_ARGUMENT;
    }
    int index=mapFind(map,key);
    if(index==ELEMENT_NOT_FOUND){
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    free(map->elements[index]); //removing the key-value
    map->elements[index] = map->elements[map->size - 1]; 
    map->size--; //set the size to the current size
    return MAP_SUCCESS;    
}

char* mapGetFirst(Map map){
    if(map == NULL){
        return NULL;
    }
    map->iterator=0;
    return mapGetNext(map);
}

char* mapGetNext(Map map){
    if(map == NULL || map->iterator >= map->size){
        return NULL;
    }
    keyValue element = map->elements[map->iterator];
    map->iterator++;
    return element->key;
}

MapResult mapClear(Map map){
    if(map == NULL){
        return MAP_NULL_ARGUMENT;
    }
    while(mapGetSize(map) > 0) {
        MapResult result=mapRemove(map, mapGetFirst(map));
        if(result != MAP_SUCCESS ){
            return MAP_ERROR;
        }
    }
    free(map->elements);
    return MAP_SUCCESS;   
}
