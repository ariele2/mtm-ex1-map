#include "election.h"
#include "mapIdList.h"
#include <stdlib.h>
#include <stdio.h>
#include "keyValue.h"
#include <assert.h>
#define LOWER_CASE_A 'a'
#define LOWER_CASE_Z 'z'
#define SPACE ' '
#define ELEMENT_NOT_FOUND -1
//destroys the election and returns the matching output message 
#define DESTROY_AND_RETURN_ELECTION(election) \
        do {\
        electionDestroy(election);\
        return ELECTION_OUT_OF_MEMORY;\
        } while(0)

//frees temporary resources inside the electionAddVote function
#define FREE_TEMP_RESOURCES \
        do { \
            free(area_string); \
            free(tribe_string); \
            free(num_of_votes_string); \
        } while(0)


struct election_t {
    Map tribes;
    Map areas;
    MapIdList votes;
};

// returns true if the tribe name is valid
static bool checkValidationTribeName(const char* tribe_name) {
    const char* tmp_ptr = tribe_name; //saving the position of the first letter
    while (*tribe_name) {
        if((*tribe_name > LOWER_CASE_Z || *tribe_name < LOWER_CASE_A)
           && *tribe_name != SPACE ) {
            return false;
        }
        tribe_name++;
    }
    tribe_name = tmp_ptr; //bringing back the pointer to the first letter
    return true;
}

//counts the digit of the entered number
static int countDigits(int number) {
    if (number == 0) {
        return 1;
    }
    int counter = 0;
    while(number > 0) {
        counter++;
        number = number/10;
    }
    return counter;
}

// converts integer to string format (char*) and returns the string
static char* convertIntToString(int number) {
    assert(number>=0);
    char* str_number= malloc(countDigits(number)+1);
    if(str_number == NULL){
        return NULL;
    }
    sprintf(str_number,"%d",number);
    return str_number;
}

// converts string format (char*) to integer and returns the integer
static int convertStringToInt(const char* string) {
    int string_to_int = 0;
    sscanf(string, "%d", &string_to_int);
    return string_to_int;
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
static ElectionResult addRemoveVoteValidation(Election election, int area_id, int tribe_id, int num_of_votes){
    if(election == NULL){
        return ELECTION_NULL_ARGUMENT;
    }
    if(area_id < 0 || tribe_id < 0){
        return ELECTION_INVALID_ID;
    }
    if(num_of_votes <= 0) {
        return ELECTION_INVALID_VOTES;
    }
    return ELECTION_SUCCESS;
}

// returns the lowest tribe id inside an election
static int calculateLowestTribeId(Election election) {
    assert(election != NULL && mapGetFirst(election->tribes) != NULL); // no need to check election or the tribes again.
    int lowest_tribe_id = convertStringToInt(mapGetFirst(election->tribes));
    MAP_FOREACH(tribes_iter, election->tribes) {
        int id_to_check = convertStringToInt(tribes_iter);
        if (id_to_check < lowest_tribe_id) {
            lowest_tribe_id = id_to_check;
        }
    }
    return lowest_tribe_id;
}

// aux function for the mapping areas to tribes - updates the areas_to_tribes_mapping with the fitting values
static bool electionComputeAreasToTribesMappingAux (Election election, Map areas_to_tribes_mapping){
    assert(election != NULL && areas_to_tribes_mapping != NULL);
    MAP_FOREACH(area_iter, election->areas) { //looping through the areas
        bool no_tribe_exists = true;
        int area_iter_int = convertStringToInt(area_iter);
        Map area_votes_map = mapIdListGetMap(election->votes, area_iter_int);
        assert (area_votes_map != NULL);
        int max_votes = 0;
        char* max_vote_tribe="";
        MAP_FOREACH(vote_tribe_iter, area_votes_map) { //looping through the vote's map
            no_tribe_exists = false;
            assert(mapGet(area_votes_map, vote_tribe_iter) != NULL); // we know there shouldnt be NULL inside
            int current_tribe_votes = convertStringToInt(mapGet(area_votes_map, vote_tribe_iter));
            if (current_tribe_votes > max_votes) {
                max_votes = current_tribe_votes;
                max_vote_tribe = vote_tribe_iter;
            }
            if(current_tribe_votes == max_votes ){ //updates the id to the smallest one
                int vote_tribe_iter_int = convertStringToInt(vote_tribe_iter);
                int max_vote_tribe_int = convertStringToInt(max_vote_tribe);
                if(vote_tribe_iter_int < max_vote_tribe_int){
                    max_vote_tribe = vote_tribe_iter;
                }
            }
        }
        if (!no_tribe_exists) { //inserting the most voted tribe in the area to the map
            MapResult result = mapPut(areas_to_tribes_mapping, area_iter, max_vote_tribe);
            if (result != MAP_SUCCESS) {
                return false;
            }
        }
        else {
            char* lowest_tribe_id_str = convertIntToString(calculateLowestTribeId(election));
            if (lowest_tribe_id_str == NULL) { //program ran out of memory
                electionDestroy(election);
                return false;
            }
            MapResult result = mapPut(areas_to_tribes_mapping, area_iter, lowest_tribe_id_str);
            if (result != MAP_SUCCESS) {
                free(lowest_tribe_id_str);
                return false;
            }
            free(lowest_tribe_id_str);
        }
    }
    return true;
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
    free(str_id);
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
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if(mapContains(election->areas,str_id)){
        free(str_id);
        return ELECTION_AREA_ALREADY_EXIST;
    }
    //at this stage we know that the key (area id) doesnt exist.
    MapResult area_map_result = mapPut(election->areas, str_id, area_name);
    if (area_map_result == MAP_OUT_OF_MEMORY) {
        free(str_id);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    MapIdListResult area_votes_result = mapIdListAdd(election->votes, area_id);
    if (area_votes_result != MAP_ID_LIST_SUCCESS) { //shouldnt enter here as we checked the inputs already
        free(str_id);
        return ELECTION_NULL_ARGUMENT;
    }
    free(str_id);
    return ELECTION_SUCCESS;
}

ElectionResult electionAddVote (Election election, int area_id, int tribe_id, int num_of_votes) {
    ElectionResult result = addRemoveVoteValidation(election, area_id, tribe_id, num_of_votes);
    if(result != ELECTION_SUCCESS) {
        return result;
    }
    char *area_string = convertIntToString(area_id); //1st allocation
    if(area_string == NULL){
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if(!mapContains(election->areas,area_string)){ 
        free(area_string);
        return ELECTION_AREA_NOT_EXIST;
    }
    char *tribe_string = convertIntToString(tribe_id); //2nd allocation
    if(tribe_string == NULL){
        free(area_string);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if(!mapContains(election->tribes,tribe_string)) { 
        free(area_string);
        free(tribe_string);
        return ELECTION_TRIBE_NOT_EXIST;
    }
    char *num_of_votes_string = convertIntToString(num_of_votes); //3rd alloaction
    if(num_of_votes_string == NULL){
        free(area_string);
        free(tribe_string);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    Map map_area_id = mapIdListGetMap(election->votes,area_id);
    if(map_area_id == NULL) {
        FREE_TEMP_RESOURCES;
        return ELECTION_NULL_ARGUMENT;
    }
    if (!mapContains(map_area_id, tribe_string)){
        if(mapPut(map_area_id, tribe_string, num_of_votes_string) != MAP_SUCCESS){
            FREE_TEMP_RESOURCES;
            DESTROY_AND_RETURN_ELECTION(election);
        }
        FREE_TEMP_RESOURCES;
        return ELECTION_SUCCESS;
    }
    int current_num_of_votes = convertStringToInt(mapGet(map_area_id, tribe_string));
    char* num_of_votes_update_string = convertIntToString(current_num_of_votes + num_of_votes); //4th allocation
    if (num_of_votes_update_string == NULL) {
        FREE_TEMP_RESOURCES;
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if (mapPut(map_area_id, tribe_string, num_of_votes_update_string) != MAP_SUCCESS) {
        FREE_TEMP_RESOURCES;
        free(num_of_votes_update_string);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    FREE_TEMP_RESOURCES;
    free(num_of_votes_update_string);
    return ELECTION_SUCCESS;
}

ElectionResult electionRemoveVote(Election election, int area_id, int tribe_id, int num_of_votes) {
    ElectionResult result = addRemoveVoteValidation(election, area_id, tribe_id, num_of_votes);
    if(result != ELECTION_SUCCESS) {
        return result;
    }
    char *area_string = convertIntToString(area_id);
    if(area_string == NULL){
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if(!mapContains(election->areas,area_string)){
        free(area_string);
        return ELECTION_AREA_NOT_EXIST;
    }
    char *tribe_string = convertIntToString(tribe_id);
    if(tribe_string == NULL){
        free(area_string);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if(!mapContains(election->tribes,tribe_string)) {
        free(area_string);
        free(tribe_string);
        return ELECTION_TRIBE_NOT_EXIST;
    }
    char *num_of_votes_string = convertIntToString(num_of_votes);
    if(num_of_votes_string == NULL){
        free(area_string);
        free(tribe_string);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    Map map_area_id = mapIdListGetMap(election->votes,area_id);
    if(map_area_id == NULL) {
        FREE_TEMP_RESOURCES;
        return ELECTION_NULL_ARGUMENT;
    }
    if (!mapContains(map_area_id, tribe_string)){
        FREE_TEMP_RESOURCES;
        return ELECTION_SUCCESS;
    }
    int current_num_of_votes = convertStringToInt(mapGet(map_area_id, tribe_string));
    if (current_num_of_votes < num_of_votes) { //if the user wants to remove more votes then the current votes, enters 0.
        num_of_votes = current_num_of_votes;
    }
    char* num_of_votes_update_string = convertIntToString(current_num_of_votes - num_of_votes);
    if (num_of_votes_update_string == NULL) {
        FREE_TEMP_RESOURCES;
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if (mapPut(map_area_id, tribe_string, num_of_votes_update_string) != MAP_SUCCESS) {
        FREE_TEMP_RESOURCES;
        free(num_of_votes_update_string);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    FREE_TEMP_RESOURCES;
    free(num_of_votes_update_string);
    return ELECTION_SUCCESS;
}

char* electionGetTribeName (Election election, int tribe_id){
    if(election == NULL){
        return NULL;
    }
    char *str_id = convertIntToString(tribe_id);
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
        free(str_id);
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
        return ELECTION_TRIBE_NOT_EXIST;
    }
    return ELECTION_SUCCESS;
}

ElectionResult electionRemoveAreas(Election election, AreaConditionFunction should_delete_area) {
    if (election == NULL || should_delete_area == NULL) {
        return ELECTION_NULL_ARGUMENT;
    }
    if (election->areas == NULL) {
        return ELECTION_NULL_ARGUMENT;
    }
    int num_of_areas = mapGetSize(election->areas);
    char** areas_to_remove = malloc(sizeof(char*)*num_of_areas);
    int areas_to_remove_counter = 0;
    MAP_FOREACH(iter, election->areas) {
        int area_id = convertStringToInt(iter);
        if (should_delete_area(area_id)) { //checking which areas need to be removed
            assert(area_id>=0);
            areas_to_remove[areas_to_remove_counter] = iter; //insert them to the array
            areas_to_remove_counter++;
        }
    }
    for (int i=0; i<areas_to_remove_counter; i++) {//looping through the array and removing
        int area_id_to_remove = convertStringToInt(areas_to_remove[i]);
        if(mapIdListRemove(election->votes, area_id_to_remove) != MAP_ID_LIST_SUCCESS) { //no need to check the id
            return ELECTION_NULL_ARGUMENT;
        }
        if(mapRemove(election->areas, areas_to_remove[i]) != MAP_SUCCESS){ //no need to check if the key exists
            return ELECTION_NULL_ARGUMENT;
        }
    }
    free(areas_to_remove);
    return ELECTION_SUCCESS;
}

Map electionComputeAreasToTribesMapping (Election election) {
    if (election == NULL) {
        return NULL;
    }
    Map areas_to_tribes_mapping = mapCreate();
    if (areas_to_tribes_mapping == NULL) {
        return NULL;
    }
    if(!electionComputeAreasToTribesMappingAux(election,areas_to_tribes_mapping)) { //no tribes in the map
        return NULL;
    }
    return areas_to_tribes_mapping;
}
