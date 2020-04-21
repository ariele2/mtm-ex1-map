#include "map.h"
#include "keyValue.h"
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

struct Map_t {
    KeyValue* elements;
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

static MapResult addOrDestroy(Map map,KeyValue element) {
    char* tmp_key = copyString(keyGet(element));
    char* tmp_value = copyString(valueGet(element));
    MapResult result = mapPut(map, tmp_key,tmp_value);
    free(tmp_key);
    free(tmp_value);
    if (result == MAP_OUT_OF_MEMORY) {
        mapDestroy(map);
    }
    return result;
}

static MapResult addAllOrDestroy(Map map, Map toAdd) {
for (int i = 0; i < toAdd->size; ++i) {
    KeyValue element=toAdd->elements[i];
    if (addOrDestroy(map,element ) == MAP_OUT_OF_MEMORY) {
        return MAP_OUT_OF_MEMORY;
    }
}
    return MAP_SUCCESS;
}

static int mapFind(Map map, const char* key) {
    for (int i = 0; i < map->size; i++) {
        //KeyValue element = map->elements[i];
        if (strcmp(keyGet(map->elements[i]), key) == 0) {
            return i;
        }
    }
 return ELEMENT_NOT_FOUND;
}

static MapResult expand(Map map) {
    int new_size = EXPAND_FACTOR * map->max_size;
    KeyValue* newElements = realloc(map->elements, new_size * sizeof(KeyValue));
    if (newElements == NULL) {
        return MAP_OUT_OF_MEMORY;
    }
    map->elements = newElements;
    map->max_size = new_size;
    return MAP_SUCCESS;
}

Map mapCreate() {
    Map map = malloc(sizeof(*map));
    if (map == NULL) {
        return NULL;
    }
    map->elements = malloc(INITIAL_SIZE * sizeof(KeyValue));
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
        free(map->elements);
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
    if (index != ELEMENT_NOT_FOUND) { //if the key exists already:
        KeyValueResult result = valueSet(map->elements[index], data);
        if (result == KEY_VALUE_NULL_ARGUMENT) {
            return MAP_NULL_ARGUMENT;
        }
        return MAP_SUCCESS;
    }
    if (map->size == map->max_size) { //if the map is full and needs a reallocation:
        if (expand(map) == MAP_OUT_OF_MEMORY) {
            return MAP_OUT_OF_MEMORY;
        }
    }
    map->elements[map->size] = keyValueCreate(); //allocates space for the new key-value
    if (map->elements[map->size] == NULL) {
        return MAP_OUT_OF_MEMORY;
    }
    if (keySet(map->elements[map->size], key) == KEY_VALUE_NULL_ARGUMENT) {
        keyValueDestroy(map->elements[map->size]);
        return MAP_NULL_ARGUMENT;
    }
    if (valueSet(map->elements[map->size], data) == KEY_VALUE_NULL_ARGUMENT) {
        keyValueDestroy(map->elements[map->size]);
        return MAP_NULL_ARGUMENT;
    }
    map->size++;
    return MAP_SUCCESS;
}

char* mapGet(Map map, const char* key){
    if(map == NULL || key == NULL){
        return NULL;
    }
    int index=mapFind(map,key);
    if(index==ELEMENT_NOT_FOUND){
        return NULL;
    }
    KeyValue element=map->elements[index];
    return valueGet(element);
}    

MapResult mapRemove(Map map, const char* key){
    if(map == NULL || key == NULL){
        return MAP_NULL_ARGUMENT;
    }
    int index = mapFind(map,key);
    if(index==ELEMENT_NOT_FOUND){
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    keyValueDestroy(map->elements[index]);
    map->elements[index] = map->elements[map->size-1];
    map->size--;
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
    KeyValue element = map->elements[map->iterator];
    map->iterator++;
    return keyGet(element);
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
    return MAP_SUCCESS;   
}
