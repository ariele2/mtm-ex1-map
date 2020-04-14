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
    keyValue** elements;
    int size;
    int max_size;
    int iterator;
};

static MapResult addOrDestroy(Map map,const keyValue* element) {
    MapResult result = mapAdd(map, element);
    if (result == MAP_OUT_OF_MEMORY) {
        mapDestroy(map);
    }
    return result;
}

static MapResult addAllOrDestroy(Map map, Map toAdd) {
for (int i = 0; i < toAdd->size; ++i) {
    if (addOrDestroy(map, toAdd->elements[i]) == MAP_OUT_OF_MEMORY) {
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
    keyValue** newElements = realloc(map->elements, new_size * sizeof(keyValue*));
    if (newElements == NULL) {
        return MAP_OUT_OF_MEMORY;
    }
    map->elements = newElements;
    map->max_size = new_size;
    return MAP_SUCCESS;
}

static char* copyString(const char* str) {
    char* newStr = malloc(strlen(str) + 1);
    if (newStr == NULL){
        return NULL;
    } 
    return strcpy(newStr, str);
} 

static MapResult createKeyValue(Map map, const char* key, const char* data) {
    // we use this function inside another one that checks for null arguments
    assert(map != NULL && key != NULL && data != NULL);
    char* new_key = copyString(key);
    char* new_value = copyString(data);
    if (new_key == NULL || new_value == NULL) {
        return MAP_OUT_OF_MEMORY;
    }
    keyValue element = map->elements[map->size];
    element->key = key;
    element->value = data;
    map->size++;
    return MAP_SUCCESS;
}

Map mapCreate() {
    Map map = malloc(sizeof(*map));
    if (map == NULL) {
        return NULL;
    }
    map->elements = malloc(INITIAL_SIZE * sizeof(keyValue*));
    if (map->elements == NULL) {
        free(map);
        return NULL;
    }

    map->size = 0;
    map->max_size = INITIAL_SIZE;
    return map;
}

void mapDestroy(Map map){
    if (map==NULL){
        return;
    }
    while(mapGetSize(map) > 0) {
        mapRemove(map, mapGetFirst(map));
        }
        free(map->elements);
        free(map);
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
    if(map==NULL){
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
        keyValue element = map->elements[index]; 
        element->value = data;
    }
    if (map->size == map->max_size) {
        if (expand(map) == MAP_OUT_OF_MEMORY) {
            return MAP_OUT_OF_MEMORY;
        }
    }
    return createKeyValue(map, key ,data);
}
