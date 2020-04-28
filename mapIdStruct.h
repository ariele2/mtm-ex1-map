#ifndef MAP_ID_STRUCT_H_
#define MAP_ID_STRUCT_H_

#include "map.h"
#include <stdbool.h>
#include <string.h>
/**
* map-id struct struct
*
* Implements an id-map struct container type.
* The type of the id is integer, and the type of the map is struct map.
* This struct holds a number, connected to a map of key-values.
*
*
* The following functions are available:
*   mapIdCreate		- Creates a new empty map-id struct.
*   mapIdDestroy	- Deletes an existing map-id, and frees all resources.
*   mapIdGetId      - returns the id of the requested element.
*   mapIdGetMap     - returns the map of the requested element.
*   mapIdSetId      - sets the id of the requested element. 
*   
*/

/** Type for defining the Map-Id list */
typedef struct mapIdStruct_t* MapId;

typedef enum MapIdResult_t {
    MAP_ID_STRUCT_SUCCESS,
    MAP_ID_STRUCT_NULL_ARGUMENT,
    MAP_ID_STRUCT_ID_NOT_VALID,
    MAP_ID_STRUCT_ERROR
} MapIdResult;

/**
* mapIdCreate: Allocates a new empty map-id struct.
*
* @return
* 	NULL - if allocations failed.
* 	A new map-id pair in case of success, initializing id to -2 in the creation proccess.
*/
MapId mapIdCreate();

/**
* mapIdDestroy: Deallocates an existing map-id element.
*
* @param mapId - Target map-id to be deallocated. If the map and the id
*                   are NULL nothing will be done.
*/
void mapIdDestroy(MapId mapId);

/**
*	mapIdGetId: Returns the Id corresponds to the entered mapId, -1 if it doesnt exist.
*
* @param mapId - The mapId element which need to be found.
* @return
*  -1 if a NULL pointer was sent or if the id does not exist.
* 	the id number otherwise.
*/
int mapIdGetId(MapId mapId);

/**
*	mapIdGetMap: Returns the map corresponds to the entered mapId, NULL if it dosent exist.
*
* @param valueElement - The key element which need to be found.
* @return
*  NULL if a NULL pointer was sent or if the value does not exist.
* 	A pointer to the map otherwise.
*/
Map mapIdGetMap(MapId mapId);
/**
 *  mapIdSetId: Set the id of the mapId entered by the user inside the id element, 
 *  returns MAP_ID_STRUCT_ERROR if it dosent succeed.
 * 
 *  @param mapId - The mapId element which need to be modified.
 *  @param idElement - The value of the id which need to insert to the mapId.
 * 
 * @return
 *  MAP_ID_STRUCT_NULL_ARGUMENT if a NULL pointer was sent.
 * 	MAP_ID_STRUCT_SUCCESS if succeeded.
 * 
 */
MapIdResult mapIdSetId(MapId mapId,int id);

/* 
 * macro MAP_ID_VALIDATION:
 * checks if the mapId entered is NULL, returns NULL if it doesnt exist.
 *
 */
#define MAP_ID_VALIDATION(mapId) \
    do { \
        if (mapId == NULL) {\
            return NULL;\
        }\
    } while(0)
    // MAP_ID_VALIDATION ends here

#endif /* MAP_LIST_STRUCT_ */