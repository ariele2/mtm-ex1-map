#include "election.h"
#include "map.h"
#include <stdlib.h>
#include <stdio.h>
#include "keyValue.h"
#define LOWER_CASE_A 'a'
#define LOWER_CASE_Z 'z'
#define SPACE ' '
#define ELEMENT_NOT_FOUND -1

struct election_t {
    Map tribes;
    Map areas;
    Map votes;
}; 


static bool checkValidationTribeName(const char* tribe_name) {
    char* tmp_ptr = tribe_name; //saving the position of the first letter
    while (tribe_name) {
        if((*tribe_name > LOWER_CASE_Z || *tribe_name < LOWER_CASE_A)
        && *tribe_name != SPACE ) {
            return false;
        }
        tribe_name++;
    }
    tribe_name = tmp_ptr; //bringing back the pointer to the first letter
    return true;
}

static int countDigits(int number) {
    int counter = 0;
    while(number > 0) {
        counter++;
        number = number/10;
    }
    return counter;
}

static char* convertIntToString(int number) {
    assert(number>=0);
    char* str_number= malloc(countDigits(number)+1);
    if(str_number == NULL){
        return NULL;
    }
    sprintf(str_number,"%d",number);
    return str_number;
}

Election electionCreate() {
    Election election = malloc(sizeof(*election));
    if (election == NULL) {
        return NULL;
    }
    election->tribes = mapCreate();
    if (election->tribes == NULL) {
        free(election);
        return NULL;
    }
    election->areas = mapCreate();
    if (election->tribes == NULL) {
        free(election->tribes);
        free(election);
        return NULL;
    }
    election->votes = mapCreate();
    if (election->votes == NULL) {
        free(election->tribes);
        free(election->areas);
        free(election);
        return NULL;
    }
    return election;
}

void electionDestroy (Election election) {
    if (election == NULL) {
        return;
    }
    mapDestroy(election->tribes);
    mapDestroy(election->areas);
    mapDestroy(election->votes); ///could be a map inside the votes!!!!!
    free(election);
}

ElectionResult electionAddTribe(Election election, int tribe_id, const char* tribe_name) {
    if(election == NULL || tribe_name == NULL) {
        return ELECTION_NULL_ARGUMENT;
    }
    if (tribe_id < 0) {
        return ELECTION_INVALID_ID;
    }
    if(!checkValidationTribeName(tribe_name)){
        return ELECTION_INVALID_NAME;
    }
    char *str_id = convertIntToString(tribe_id);
    if(str_id == NULL){
        electionDestroy(election);
        return ELECTION_OUT_OF_MEMORY;
    }
    if(mapContains(election->tribes,str_id)){
        free(str_id);
        return ELECTION_TRIBE_ALREADY_EXIST;
    }
    //at this stage we know that the key dosent exist.
    MapResult result = mapPut(election->tribes, str_id, tribe_name);
    if (result == MAP_OUT_OF_MEMORY) {
        free(str_id);
        electionDestroy(election);
        return ELECTION_OUT_OF_MEMORY;
    }
    return ELECTION_SUCCESS;
}

ElectionResult electionAddArea(Election election, int area_id, const char* area_name){
    if(election == NULL || area_name == NULL) {
        return ELECTION_NULL_ARGUMENT;
    }
    if (area_id < 0) {
        return ELECTION_INVALID_ID;
    }
    if(!checkValidationTribeName(area_name)){
        return ELECTION_INVALID_NAME;
    }
    char *str_id = convertIntToString(area_id);
    if(str_id == NULL){
        free(str_id);
        electionDestroy(election);
        return ELECTION_OUT_OF_MEMORY;
    }
    if(mapContains(election->areas,str_id)){
        free(str_id);
        return ELECTION_TRIBE_ALREADY_EXIST;
    }
    //at this stage we know that the key dosent exist.
    MapResult result = mapPut(election->areas, str_id, area_name);
    if (result == MAP_OUT_OF_MEMORY) {
        free(str_id);
        electionDestroy(election);
        return ELECTION_OUT_OF_MEMORY;
    }
    return ELECTION_SUCCESS;
}

char* electionGetTribeName (Election election, int tribe_id){
    if(election == NULL){
        return NULL;
    }
    char *str_id = convertIntToString(tribe_id);  ///check if "const" necessary
    if(str_id == NULL){
        return NULL;
    }
    char *str_name = mapGet(election->tribes,str_id);
    free(str_id);
    char *returned_name = malloc(strlen(str_name)+1); ///need to free this later
    return strcpy(returned_name,str_name);
}

ElectionResult electionSetTribeName (Election election, int tribe_id, const char* tribe_name){
    if(election == NULL || tribe_name == NULL) {
        return ELECTION_NULL_ARGUMENT;
    }
    if (tribe_id < 0) {
        return ELECTION_INVALID_ID;
    }
    if(!checkValidationTribeName(tribe_name)){
        return ELECTION_INVALID_NAME;
    }
    char *str_id = convertIntToString(tribe_id);
    if(str_id == NULL){
        electionDestroy(election);
        return ELECTION_OUT_OF_MEMORY;
    }

    if(!mapContains(election->tribes,str_id)){
        return ELECTION_TRIBE_NOT_EXIST;
    }

    MapResult result = mapPut(election->tribes,str_id,tribe_name);
    free(str_id);

    if(result == MAP_NULL_ARGUMENT){
        return ELECTION_NULL_ARGUMENT;
    }

    if(result == MAP_OUT_OF_MEMORY){
        electionDestroy(election);
        return ELECTION_OUT_OF_MEMORY;
    }

    return ELECTION_SUCCESS;
}

ElectionResult electionRemoveTribe (Election election, int tribe_id){
    if(election == NULL) {
        return ELECTION_NULL_ARGUMENT;
    }
    if (tribe_id < 0) {
        return ELECTION_INVALID_ID;
    }
    char *str_id = convertIntToString(tribe_id);
    if(str_id == NULL){
        electionDestroy(election);
        return ELECTION_OUT_OF_MEMORY;
    }
    MapResult result=mapRemove(election->tribes,str_id);
    free(str_id);

    if(result == MAP_NULL_ARGUMENT){
        return ELECTION_NULL_ARGUMENT;
    }

    if(result == MAP_ITEM_DOES_NOT_EXIST){
        ELECTION_TRIBE_NOT_EXIST;
    }

    return ELECTION_SUCCESS;

}