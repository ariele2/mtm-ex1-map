#include "mapIdList.h"
#include "mapIdStruct.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#define ID_ERROR -1

struct mapIdList_t {
    MapId mapId;
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
    mapIdList = mapIdList->next; //no need to check the birtual head node
    while(mapIdList) {
        int node_id = mapIdGetId(mapIdList->mapId);
        if (node_id == ID_ERROR) {
            return NULL;
        }
        if (id == node_id) {
            return mapIdList;
        }
        mapIdList = mapIdList->next;
    }
    return NULL; //id dosent exists if reached here
}

//destroys a given MapIdList node
static void nodeDestroy(MapIdList node) {
    assert(node !=NULL); //node shouldnt be NULL as the calling function checked it already
    mapIdDestroy(node->mapId);
    free(node);
}

MapIdList mapIdListCreate() { //creates a first virtual head node with id: -1
    MapIdList mapIdList = malloc(sizeof(*mapIdList));
    if (mapIdList == NULL) {
        return NULL;
    }
    mapIdList->mapId = mapIdCreate();
    if (mapIdList->mapId == NULL) {
        free(mapIdList);
        return NULL;
    }
    mapIdList->next = NULL;
    return mapIdList;
}

void mapIdListDestroy(MapIdList mapIdList) {
    if (!mapIdList) { //recursive solution
        return;
    }
    mapIdListDestroy(mapIdList->next);
    mapIdDestroy(mapIdList->mapId);
    free(mapIdList);
}

MapIdListResult mapIdListAdd(MapIdList mapIdList, int id) {
    ELEMENTS_VALIDATION(mapIdList, id);
    MapIdList new_mapIdList = mapIdListCreate();
    if (new_mapIdList == NULL) {
        return MAP_ID_LIST_NULL_ARGUMENT;
    }
    MapIdResult result = mapIdSetId(new_mapIdList->mapId, id);
    if (result != MAP_ID_STRUCT_SUCCESS) {
        return MAP_ID_LIST_ERROR;
    }
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
    assert(prev_node != NULL); //it cant be NULL so we put assert here
    prev_node->next = node_to_remove->next;
    nodeDestroy(node_to_remove);
    return MAP_ID_LIST_SUCCESS; 
}

Map mapIdListGetMap(MapIdList mapIdList, int id) {
    if(mapIdList == NULL || !(checkValidId(id))) {
        return NULL;
    }
    MapIdList node_to_find = findNodeById(mapIdList, id);
    if (node_to_find == NULL) {
        return NULL;
    }
    return mapIdGetMap(node_to_find->mapId);
}