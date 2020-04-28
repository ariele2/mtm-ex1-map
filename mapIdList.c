#include "mapIdList.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

struct mapIdList_t {
    int id;
    Map map;
    struct mapIdList_t* next;
};

// checks validation of the id entered, returns true if the id is valid
static inline bool checkValidId (int id) {
    if (id >= 0) {
        return true;
    }
    return false;
}

// finds the previous node to a specific node inside a list.
static MapIdList findPreviousNode(MapIdList mapIdList, MapIdList node) {
    //No NULL arguments here as the function calling this one checked it already
    assert(mapIdList != NULL && node !=NULL);
    while(mapIdList) {
        if (mapIdList->next == node) {
            return mapIdList;
        }
        mapIdList = mapIdList->next;
    }
    return NULL; //shouldnt get here - any node has a previous one except the virtual head
}

static MapIdList findNodeById (MapIdList mapIdList, int id) {
    assert(mapIdList != NULL && checkValidId(id)); 
    while(mapIdList) {
        if (id == mapIdList->id) {
            return mapIdList;
        }
        mapIdList = mapIdList->next;
    }
    return NULL; //id dosent exists if reached here
}

//destroys a give MapIdList node
static void nodeDestroy(MapIdList node) {
    assert(node !=NULL); //node shouldnt be NULL as the calling function checked it already
    mapDestroy(node->map);
    free(node);
}

MapIdList mapIdListCreate() { //creates a first virtual head node with id: -1
    MapIdList mapIdList = malloc(sizeof(*mapIdList));
    if (mapIdList == NULL) {
        return NULL;
    }
    mapIdList->map = mapCreate();
    if (mapIdList->map == NULL) {
        free (mapIdList);
        return NULL;
    }
    mapIdList->id = -1;
    mapIdList->next = NULL;
    return mapIdList;
}

void mapIdListDestroy(MapIdList mapIdList) {
    if (!mapIdList) { //recursive solution
        return;
    }
    mapIdListDestroy(mapIdList->next);
    mapDestroy(mapIdList->map);
    free(mapIdList);
}

MapIdListResult mapIdListAdd(MapIdList mapIdList, int id) {
    ELEMENTS_VALIDATION(mapIdList, id);
    MapIdList new_mapIdList = mapIdListCreate();
    if (new_mapIdList == NULL) {
        return MAP_ID_LIST_ERROR;
    }
    new_mapIdList->id = id;
    MapIdList ptr = mapIdList;
    while(ptr->next) { //looping until reaching the last node in the list
        ptr = ptr->next;
    }
    ptr->next = new_mapIdList; //connects the new node.
    return MAP_ID_LIST_SUCCESS;
}

MapIdListResult mapIdListRemove(MapIdList mapIdList, int id) {
    ELEMENTS_VALIDATION(mapIdList, id);
    MapIdList node_to_remove = findNodeById(mapIdList, id); 
    if (node_to_remove == NULL) { // means that the id dosent exists
        return MAP_ID_LIST_SUCCESS;
    }
    MapIdList prev_node = findPreviousNode(mapIdList, node_to_remove);
    prev_node->next = node_to_remove->next;
    nodeDestroy(node_to_remove);
    return MAP_ID_LIST_SUCCESS; //if it reached here it means the id dosent exists
}

Map mapIdListGetMap(MapIdList mapIdList, int id) {
    if(mapIdList == NULL || !(checkValidId(id))) {
        return NULL;
    }
    MapIdList node_to_find = findNodeById(mapIdList, id);
    return node_to_find->map;
}