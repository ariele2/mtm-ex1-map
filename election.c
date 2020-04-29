#include "election.h"
#include "mapIdList.h"
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
    MapIdList votes;
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

//checks the inputs: election - not null, tribe_id - not negative, tribe_name - not null and valid.
static ElectionResult tribeAddSetValidation (Election election, int tribe_id, const char* tribe_name) {
    if(election == NULL || tribe_name == NULL) {
        return ELECTION_NULL_ARGUMENT;
    }
    if (tribe_id < 0) {
        return ELECTION_INVALID_ID;
    }
    if(!checkValidationTribeName(tribe_name)){
        return ELECTION_INVALID_NAME;
    }
    return ELECTION_SUCCESS;
}

//checks the inputs: election - not null, tribe_id, area_id and num_of_votes - not negative
static ElectionResult addVoteValidation(Election election, int area_id, int tribe_id, int num_of_votes){
    if(election == NULL){
        ELECTION_NULL_ARGUMENT;
    }
    if(area_id < 0 || tribe_id < 0){
        ELECTION_NULL_ARGUMENT;
    }
    if(num_of_votes <= 0) {
        return ELECTION_INVALID_VOTES;
    }
    return ELECTION_SUCCESS;
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
        mapDestroy(election->tribes);
        free(election);
        return NULL;
    }
    election->votes = mapIdListCreate();
    if (election->votes == NULL) {
        mapDestroy(election->tribes);
        mapDestroy(election->areas);
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
    mapIdListDestroy(election->votes);
    free(election);
}

ElectionResult electionAddTribe(Election election, int tribe_id, const char* tribe_name) {
    ElectionResult validation = tribeAddSetValidation(election, tribe_id, tribe_name);
    if (validation != ELECTION_SUCCESS) {
        return validation;
    }
    char *str_id = convertIntToString(tribe_id);
    if(str_id == NULL){
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if(mapContains(election->tribes,str_id)){
        free(str_id);
        return ELECTION_TRIBE_ALREADY_EXIST;
    }
    //at this stage we know that the key (tribe id) doesnt exist.
    MapResult result = mapPut(election->tribes, str_id, tribe_name);
    if (result == MAP_OUT_OF_MEMORY) {
        free(str_id);
        DESTROY_AND_RETURN_ELECTION(election);
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
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if(mapContains(election->areas,str_id)){
        free(str_id);
        return ELECTION_TRIBE_ALREADY_EXIST;
    }
    //at this stage we know that the key (area id) doesnt exist.
    MapResult area_map_result = mapPut(election->areas, str_id, area_name);
    if (area_map_result == MAP_OUT_OF_MEMORY) {
        free(str_id);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    MapIdListResult area_votes_result = mapIdListAdd(election->votes, area_id);
    if (area_votes_result != MAP_ID_LIST_SUCCESS) { //shouldnt enter here as we checked the inputs already
        return ELECTION_NULL_ARGUMENT;
    }
    return ELECTION_SUCCESS;
}

ElectionResult electionAddVote (Election election, int area_id, int tribe_id, int num_of_votes) {
    ElectionResult result = addVoteValidation(election, area_id, tribe_id, num_of_votes);
    if(result != ELECTION_SUCCESS) {
        return result;
    }
    const char *area_string = convertIntToString(area_id);
    if(area_string == NULL){
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if(!mapContains(election->areas,area_string)){
        free(area_string);
        return ELECTION_AREA_NOT_EXIST;
    }
    const char *tribe_string = convertIntToString(tribe_id);
    if(tribe_string == NULL){
        free(area_string);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if(!mapContains(election->tribes,tribe_string)) {
        free(area_string);
        free(tribe_string);
        return ELECTION_TRIBE_NOT_EXIST;
    }
    const char *num_of_votes_string = convertIntToString(num_of_votes);
    if(num_of_votes_string == NULL){
        free(area_string);
        free(tribe_string);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    Map map_area_id = mapIdListGetMap(election->votes,area_id);
    if(map_area_id == NULL) {
        return ELECTION_NULL_ARGUMENT;
    }
    if (!mapContains(map_area_id, tribe_string)){ 
        if(mapPut(map_area_id, tribe_string, num_of_votes_string) != MAP_SUCCESS){
            FREE_TEMP_RESOURCES;
            DESTROY_AND_RETURN_ELECTION(election);
        }
        return ELECTION_SUCCESS;       
    }
    int current_num_of_votes = atoi(mapGet(map_area_id, tribe_string));
    char* num_of_votes_update_string = convertIntToString(current_num_of_votes + num_of_votes);
    if (num_of_votes_update_string == NULL) {
        FREE_TEMP_RESOURCES;
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if (mapPut(map_area_id, tribe_string, num_of_votes_update_string) != MAP_SUCCESS) {
        FREE_TEMP_RESOURCES;
        free(num_of_votes_update_string);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    return ELECTION_SUCCESS;
}

char* electionGetTribeName (Election election, int tribe_id){
    if(election == NULL){
        return NULL;
    }
    const char *str_id = convertIntToString(tribe_id);  //const for the mapGet function
    if(str_id == NULL){
        return NULL;
    }
    char *str_name = mapGet(election->tribes,str_id);
    free(str_id);
    char *returned_name = malloc(strlen(str_name)+1); //assuming the user frees it later 
    return strcpy(returned_name,str_name);
}

ElectionResult electionSetTribeName (Election election, int tribe_id, const char* tribe_name){
    ElectionResult validation = tribeAddSetValidation(election, tribe_id, tribe_name);
    if (validation != ELECTION_SUCCESS) {
        return validation;
    }
    char *str_id = convertIntToString(tribe_id);
    if(str_id == NULL){
        DESTROY_AND_RETURN_ELECTION(election);
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
        DESTROY_AND_RETURN_ELECTION(election);
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
        DESTROY_AND_RETURN_ELECTION(election);
    }
    MapResult result = mapRemove(election->tribes,str_id);
    free(str_id);
    if(result == MAP_NULL_ARGUMENT){
        return ELECTION_NULL_ARGUMENT;
    }
    if(result == MAP_ITEM_DOES_NOT_EXIST){
        ELECTION_TRIBE_NOT_EXIST;
    }
    return ELECTION_SUCCESS;
}

ElectionResult electionRemoveAreas(Election election, AreaConditionFunction should_delete_area) {
    if (election == NULL || should_delete_area == NULL) {
        return ELECTION_NULL_ARGUMENT;
    }
    MAP_FOREACH(iter, election->areas) {
        int area_id = atoi(iter);
        if (should_delete_area(area_id)) {
            assert(area_id>=0);
            if(mapIdListRemove(election->votes, area_id) != MAP_ID_LIST_SUCCESS) { //no need to check the id
                return ELECTION_NULL_ARGUMENT;
            }
            if(mapRemove(election->areas, iter) != MAP_SUCCESS){ //no need to check if the key exists
                return MAP_NULL_ARGUMENT;
            }
        }
    }
    return ELECTION_SUCCESS;
}

