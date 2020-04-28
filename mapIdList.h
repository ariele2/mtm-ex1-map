#ifndef MAP_ID_LIST_H_
#define MAP_ID_LIST_H_

#include "map.h"
#include <stdbool.h>
#include <string.h>
/**
* List of id-map struct
*
* Implements a list of id-map struct container type.
* The type of the id is integer, and the type of the map is struct map.
* This struct holds a number, connected to a map of key-values.
*
* !Notice!: whenever there is a call to the mapIdList element (as a list),
*         it means that the function expects to recieve the head of the list. 
*
* The following functions are available:
*   mapIdListCreate		- Creates a new empty map-id list struct
*   mapIdListDestroy	- Deletes an existing map-id list, and frees all resources
*   mapIdListAdd        - Creates a new node to an existing list.
*   mapIdListRemove     - Removes an existing node (by id). 
*   mapIdListGetMap  	- Returns the requested map, connected to the id entered.
*/

/** Type for defining the Map-Id list */
typedef struct mapIdList_t* MapIdList;

typedef enum MapIdListResult_t {
    MAP_ID_LIST_SUCCESS,
    MAP_ID_LIST_NULL_ARGUMENT,
    MAP_ID_LIST_ID_NOT_VALID,
    MAP_ID_LIST_ERROR
} MapIdListResult;

/**
* mapIdListCreate: Allocates a new head node of the struct (list of map-id).
*                  and putting an id= -1, for identification of the head node. 
*
* @return
* 	NULL - if allocations failed/ invalid id entered.
* 	A new id-map node in case of success.
*/
MapIdList mapIdListCreate();

/**
* mapIdListDestroy: Deallocates an existing map-id list.
*
* @param mapIdList - Target list to be deallocated. If the list is NULL nothing will be done.
*                 
*/
void mapIdListDestroy(MapIdList mapIdList);

/**
*	mapIdListAdd: Adds a node to the last element in the current list.
*
* @param mapIdList - The list which the node will be added to. 
* @param idElement - The id element which will be set to the node's id.
* @return
*  MAP_ID_LIST_NULL_ARGUMENT if a NULL pointer was sent or if the id is not valid.
*  MAP_ID_LIST_SUCCESS otherwise.
*/
MapIdListResult mapIdListAdd(MapIdList mapIdList, int id);

/**
*	mapIdListRemove: Removes a node conatining the entered id from the current list.
*
* @param mapIdList - The list which the node will be removed from. 
* @param idElement - The id element which will tell us what node to remove.
* @return
*  MAP_ID_LIST_NULL_ARGUMENT if a NULL pointer was sent or if the id is not valid.
*  MAP_ID_LIST_SUCCESS if the id does not exist, or if the operation succeded.
*/
MapIdListResult mapIdListRemove(MapIdList mapIdList, int id);

/**
*	mapIdListGetMap: accessing the node by id, and returning the map inside of it
*                    for further uses of the map (updateing the map).
*
* @param mapIdList - The list which will be searched for such node conataining the id entered. 
* @param idElement - The id element which will tell us the node we are returning the map from.
* @return
*  NULL if a NULL pointer was sent or if the id is not valid.
*  the map inside the node otherwise.
*/
Map mapIdListGetMap(MapIdList mapIdList, int id);

/* 
 * macro ELEMENTS_VALIDATION:
 * checks if the mapIdList entered is NULL and if the id is valid and returns
 * the appropriate messege if needed.
 *
 */
#define ELEMENTS_VALIDATION(item, id) \
    do { \
        if (item == NULL) {\
            return MAP_ID_LIST_NULL_ARGUMENT;\
        }\
        if (!checkValidId(id)) {\
            return MAP_ID_LIST_ID_NOT_VALID;\
        }\
    } while(0)
    // ELEMENTS_VALIDATION ends here

#endif /* MAP_LIST_H_ */