#include "mapIdStruct.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#define ID_ERROR -1
#define INIT_ID -2

struct mapIdStruct_t {
    int id;
    Map map;
};

MapId mapIdCreate() {
    MapId mapId = malloc(sizeof(*mapId));
    if (mapId == NULL) {
        return NULL;
    }
    mapId->map = mapCreate();
    MAP_ID_VALIDATION(mapId->map);
    mapId->id = INIT_ID;
    return mapId;
}

void mapIdDestroy(MapId mapId) {
    if (mapId == NULL) {
        return;
    }
    mapDestroy(mapId->map);
    free(mapId);
}

int mapIdGetId(MapId mapId) {
    if (mapId == NULL) {
        return ID_ERROR;
    }
    return mapId->id;
}

Map mapIdGetMap(MapId mapId) {
    MAP_ID_VALIDATION(mapId);
    return mapId->map;
}

MapIdResult mapIdSetId(MapId mapId, int id) {
    if (id <= ID_ERROR) {
        return MAP_ID_STRUCT_ID_NOT_VALID;
    }
    if (mapId == NULL) {
        return MAP_ID_STRUCT_NULL_ARGUMENT;
    }
    mapId->id = id;
    return MAP_ID_STRUCT_SUCCESS;
}