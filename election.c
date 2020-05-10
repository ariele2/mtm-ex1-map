#include "election.h"
#include "mapIdList.h"
#include <stdlib.h>
#include <stdio.h>
#include "keyValue.h"
#include <assert.h>
#include "utils.h"
#define LOWER_CASE_A 'a'
#define LOWER_CASE_Z 'z'
#define SPACE ' '
#define ELEMENT_NOT_FOUND -1
#define ADD_REMOVE_VOTE_VARIABLES 3
#define FIRST_ELEMENT 0
#define SECOND_ELEMENT 1
#define THIRD_ELEMENT 2

//destroys the election and returns the matching output message 
#define DESTROY_AND_RETURN_ELECTION(election) \
        do { \
            electionDestroy(election); \
            return ELECTION_OUT_OF_MEMORY;\
        } while(0) 


//frees temporary resources var1, var2, var3. enter NULL if there is no need of one of them.
#define FREE_TEMP_RESOURCES(var1,var2,var3) \
        do { \
            freeResource(var1); \
            freeResource(var2); \
            freeResource(var3); \
        } while(0)


//checks if sscanf worked properly. frees the resources if there is a need to free.
#define SSCANF_CHECK_AND_FREE(element, to_free, to_return) \
        do { \
            if (element == ELEMENT_NOT_FOUND) { \
                freeResource(to_free); \
                return to_return; \
            } \
        } while(0)

//validate the resources of function election add/remove vote.
#define VOTE_RESOURCES_VALIDATATION \
        do { \
            ElectionResult validation_result = addRemoveVoteValidation(election, area_id, tribe_id, num_of_votes); \
            if(validation_result != ELECTION_SUCCESS) { \
                return validation_result; \
            } \
        } while(0)

//validate that the allocations while casting from int to str works properly, also checks if tribe&area exists
//should insert true inside boolean while using the macro inside add, false inside remove. 
#define VOTE_STR_ALLOCATION_AND_CHECK(boolean) \
        do { \
            ElectionResult result = allocateStrVariables(election, area_id, tribe_id, \
                            num_of_votes, string_variables, boolean); \
            if (result != ELECTION_SUCCESS) { \
                return result; \
            } \
        } while(0)
        
struct election_t {
    Map tribes;
    Map areas;
    MapIdList votes;
};

//checks if the user entered NULL as the argument to free, and if it is NULL it wont do anything.
static inline void freeResource(void* resource){
    if (resource != NULL) {
        free(resource);
    } 
}

// checks the tribe and area name and returns true if the tribe/area name is valid
static bool checkValidationTribeOrAreaName(const char* name) {
    const char* tmp_ptr = name; //saving the position of the first letter
    while (*name) {
        if((*name > LOWER_CASE_Z || *name < LOWER_CASE_A)
           && *name != SPACE ) {
            return false;
        }
        name++;
    }
    name = tmp_ptr; //bringing back the pointer to the first letter
    return true;
}

//counts the digit of the entered number and returns the counter
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
    if (number < 0) {
        return NULL;
    }
    char* str_number= malloc(countDigits(number)+1);
    if(str_number == NULL){
        return NULL;
    }
    if (sprintf(str_number,"%d",number) < 0) {
        FREE_TEMP_RESOURCES(str_number,NULL,NULL);
        return NULL;
    }
    return str_number;
}

// converts string format (char*) to integer and returns the integer
static int convertStringToInt(const char* string) {
    int string_to_int = 0;
    if (sscanf(string, "%d", &string_to_int) == EOF) {
        return ELEMENT_NOT_FOUND;
    }
    return string_to_int;
}

//checks the inputs: election - not null, id - not negative, name - not null. (id&name of the tribe or area).
static ElectionResult addOrSetValidation (Election election, int id, const char* name) {
    if(election == NULL || name == NULL) {
        return ELECTION_NULL_ARGUMENT;
    }
    if (id < 0) {
        return ELECTION_INVALID_ID;
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

// allocates the string variables for the add/remove votes function, and checks if the area/tribe exists already
static ElectionResult allocateStrVariables(Election election, int area_id, int tribe_id,
                                                int num_of_votes, char** string_variables, bool add_or_remove) {
    string_variables[FIRST_ELEMENT] = convertIntToString(area_id);
    if(string_variables[FIRST_ELEMENT] == NULL){
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if(!mapContains(election->areas,string_variables[FIRST_ELEMENT])){
        FREE_TEMP_RESOURCES(string_variables[FIRST_ELEMENT],NULL,NULL);
        return ELECTION_AREA_NOT_EXIST;
    }
    string_variables[SECOND_ELEMENT] = convertIntToString(tribe_id);
    if(string_variables[SECOND_ELEMENT] == NULL){
        FREE_TEMP_RESOURCES(string_variables[FIRST_ELEMENT],NULL,NULL);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if(!mapContains(election->tribes,string_variables[SECOND_ELEMENT])) {
        FREE_TEMP_RESOURCES(string_variables[FIRST_ELEMENT],string_variables[SECOND_ELEMENT],NULL);
        return ELECTION_TRIBE_NOT_EXIST;
    }
    if(add_or_remove){
        string_variables[THIRD_ELEMENT] = convertIntToString(num_of_votes);
        if(string_variables[THIRD_ELEMENT] == NULL){
            FREE_TEMP_RESOURCES(string_variables[FIRST_ELEMENT],string_variables[SECOND_ELEMENT],NULL);
            DESTROY_AND_RETURN_ELECTION(election);
        }
    }
    FREE_TEMP_RESOURCES(string_variables[FIRST_ELEMENT],NULL,NULL);
    return ELECTION_SUCCESS;
}

//finds the lowest tribe id and returns the id inside a given election
static int calculateLowestTribeId(Election election) {
    assert(election != NULL && mapGetFirst(election->tribes) != NULL); // no need to check election or the tribes again.
    int lowest_tribe_id = convertStringToInt(mapGetFirst(election->tribes));
    SSCANF_CHECK_AND_FREE(lowest_tribe_id,NULL, ELEMENT_NOT_FOUND); //no need to free
    MAP_FOREACH(tribes_iter, election->tribes) {
        int id_to_check = convertStringToInt(tribes_iter);
        SSCANF_CHECK_AND_FREE(id_to_check,NULL, ELEMENT_NOT_FOUND); //no need to free
    }

    return lowest_tribe_id;
}

// aux function for the mapping areas to tribes - updates the areas_to_tribes_mapping with the fitting values
static bool electionComputeAreasToTribesMappingAux (Election election, Map areas_to_tribes_mapping){
    assert(election != NULL && areas_to_tribes_mapping != NULL);
    MAP_FOREACH(area_iter, election->areas) { //looping through the areas
        bool no_tribe_exists = true;
        int area_iter_int = convertStringToInt(area_iter);
        SSCANF_CHECK_AND_FREE(area_iter_int,NULL, false);
        Map area_votes_map = mapIdListGetMap(election->votes, area_iter_int); // the map of the current area_id
        assert (area_votes_map != NULL); //shouldnt be NULL
        int max_votes = 0;
        char* max_vote_tribe="";
        MAP_FOREACH(vote_tribe_iter, area_votes_map) { //looping through the vote's map
            no_tribe_exists = false;
            assert(mapGet(area_votes_map, vote_tribe_iter) != NULL); // we know there shouldnt be NULL inside
            int current_tribe_votes = convertStringToInt(mapGet(area_votes_map, vote_tribe_iter));
            SSCANF_CHECK_AND_FREE(current_tribe_votes,NULL, false);
            if (current_tribe_votes > max_votes) {
                max_votes = current_tribe_votes;
                max_vote_tribe = vote_tribe_iter;
            }
            if(current_tribe_votes == max_votes ){ //updates the id to the smallest one
                int vote_tribe_iter_int = convertStringToInt(vote_tribe_iter);
                SSCANF_CHECK_AND_FREE(vote_tribe_iter_int,NULL, false);
                int max_vote_tribe_int = convertStringToInt(max_vote_tribe);
                SSCANF_CHECK_AND_FREE(max_vote_tribe_int,NULL, false);
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
                FREE_TEMP_RESOURCES(lowest_tribe_id_str,NULL,NULL);
                return false;
            }
            FREE_TEMP_RESOURCES(lowest_tribe_id_str,NULL,NULL);
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
    ElectionResult validation = addOrSetValidation(election, tribe_id, tribe_name);
    if (validation != ELECTION_SUCCESS) {
        return validation;
    }
    char *str_id = convertIntToString(tribe_id);
    if(str_id == NULL){
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if(mapContains(election->tribes,str_id)){
        FREE_TEMP_RESOURCES(str_id,NULL,NULL);
        return ELECTION_TRIBE_ALREADY_EXIST;
    }
    if(!checkValidationTribeOrAreaName(tribe_name)){
        FREE_TEMP_RESOURCES(str_id,NULL,NULL);
        return ELECTION_INVALID_NAME;
    }
    //at this stage we know that the key (tribe id) doesnt exist.
    MapResult result = mapPut(election->tribes, str_id, tribe_name);
    if (result == MAP_OUT_OF_MEMORY) {
        FREE_TEMP_RESOURCES(str_id,NULL,NULL);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    FREE_TEMP_RESOURCES(str_id,NULL,NULL);
    return ELECTION_SUCCESS;
}


ElectionResult electionAddArea(Election election, int area_id, const char* area_name){
    ElectionResult result = addOrSetValidation(election,area_id, area_name);
    if(result != ELECTION_SUCCESS){
        return result;
    }
    char *str_id = convertIntToString(area_id);
    if(str_id == NULL){
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if(mapContains(election->areas,str_id)){
        FREE_TEMP_RESOURCES(str_id,NULL,NULL);
        return ELECTION_AREA_ALREADY_EXIST;
    }
    if(!checkValidationTribeOrAreaName(area_name)){
        FREE_TEMP_RESOURCES(str_id,NULL,NULL);
        return ELECTION_INVALID_NAME;
    }
    //at this stage we know that the key (area id) doesnt exist.
    MapResult area_map_result = mapPut(election->areas, str_id, area_name);
    if (area_map_result == MAP_OUT_OF_MEMORY) {
        FREE_TEMP_RESOURCES(str_id,NULL,NULL);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    MapIdListResult area_votes_result = mapIdListAdd(election->votes, area_id);
    if (area_votes_result != MAP_ID_LIST_SUCCESS) { //shouldnt enter here as we checked the inputs already
        FREE_TEMP_RESOURCES(str_id,NULL,NULL);
        return ELECTION_NULL_ARGUMENT;
    }
    FREE_TEMP_RESOURCES(str_id,NULL,NULL);
    return ELECTION_SUCCESS;
}

ElectionResult electionAddVote (Election election, int area_id, int tribe_id, int num_of_votes) {
    VOTE_RESOURCES_VALIDATATION;
    char* string_variables[ADD_REMOVE_VOTE_VARIABLES]; //contains the string variables we need to use
    VOTE_STR_ALLOCATION_AND_CHECK(true);
    char* tribe_string = string_variables[SECOND_ELEMENT];
    char* num_of_votes_string = string_variables[THIRD_ELEMENT];
    Map map_area_id = mapIdListGetMap(election->votes,area_id);
    if(map_area_id == NULL) {
        FREE_TEMP_RESOURCES(tribe_string,num_of_votes_string,NULL);
        return ELECTION_NULL_ARGUMENT;
    }
    if (!mapContains(map_area_id, tribe_string)){
        if(mapPut(map_area_id, tribe_string, num_of_votes_string) != MAP_SUCCESS){
            FREE_TEMP_RESOURCES(tribe_string,num_of_votes_string,NULL);
            DESTROY_AND_RETURN_ELECTION(election);
        }
        FREE_TEMP_RESOURCES(tribe_string,num_of_votes_string,NULL);
        return ELECTION_SUCCESS;
    }
    int current_num_of_votes = convertStringToInt(mapGet(map_area_id, tribe_string));
    if (current_num_of_votes == ELEMENT_NOT_FOUND) {
        FREE_TEMP_RESOURCES(tribe_string,num_of_votes_string,NULL);
        return ELECTION_ERROR;
    }
    char* num_of_votes_update_string = convertIntToString(current_num_of_votes + num_of_votes);
    if (num_of_votes_update_string == NULL) {
        FREE_TEMP_RESOURCES(tribe_string,num_of_votes_string,NULL);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if (mapPut(map_area_id, tribe_string, num_of_votes_update_string) != MAP_SUCCESS) {
        FREE_TEMP_RESOURCES(tribe_string,num_of_votes_string,num_of_votes_update_string);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    FREE_TEMP_RESOURCES(tribe_string,num_of_votes_string,num_of_votes_update_string);
    return ELECTION_SUCCESS;
}

ElectionResult electionRemoveVote(Election election, int area_id, int tribe_id, int num_of_votes) {
    VOTE_RESOURCES_VALIDATATION; 
    char* string_variables[ADD_REMOVE_VOTE_VARIABLES];
    VOTE_STR_ALLOCATION_AND_CHECK(false);
    char* tribe_string = string_variables[SECOND_ELEMENT];
    Map map_area_id = mapIdListGetMap(election->votes,area_id);
    if(map_area_id == NULL) {
        FREE_TEMP_RESOURCES(tribe_string,NULL,NULL);
        return ELECTION_NULL_ARGUMENT;
    }
    if (!mapContains(map_area_id, tribe_string)){
        FREE_TEMP_RESOURCES(tribe_string,NULL,NULL);
        return ELECTION_SUCCESS;
    }
    int current_num_of_votes = convertStringToInt(mapGet(map_area_id, tribe_string));
    SSCANF_CHECK_AND_FREE(current_num_of_votes, tribe_string, ELECTION_ERROR);
    if (current_num_of_votes < num_of_votes) { //if the user removes more votes then the current votes, enters 0.
        num_of_votes = current_num_of_votes;
    }
    char* num_of_votes_update_string = convertIntToString(current_num_of_votes - num_of_votes);
    if (num_of_votes_update_string == NULL) {
        FREE_TEMP_RESOURCES(tribe_string,NULL,NULL);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if (mapPut(map_area_id, tribe_string, num_of_votes_update_string) != MAP_SUCCESS) {
        FREE_TEMP_RESOURCES(tribe_string,num_of_votes_update_string,NULL);
        DESTROY_AND_RETURN_ELECTION(election);
    }
    FREE_TEMP_RESOURCES(tribe_string,num_of_votes_update_string,NULL);
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
    if (str_name == NULL) {
        FREE_TEMP_RESOURCES(str_id,NULL,NULL);
        return NULL;
    }
    FREE_TEMP_RESOURCES(str_id,NULL,NULL);
    char *returned_name = malloc(strlen(str_name)+1); //assuming the user frees it later
    if(returned_name == NULL){
        return NULL;
    }
    return strcpy(returned_name,str_name);
}

ElectionResult electionSetTribeName (Election election, int tribe_id, const char* tribe_name){
    ElectionResult validation = addOrSetValidation(election, tribe_id, tribe_name);
    if (validation != ELECTION_SUCCESS) {
        return validation;
    }
    char *str_id = convertIntToString(tribe_id);
    if(str_id == NULL){
        DESTROY_AND_RETURN_ELECTION(election);
    }
    if(!mapContains(election->tribes,str_id)){
        FREE_TEMP_RESOURCES(str_id,NULL,NULL);
        return ELECTION_TRIBE_NOT_EXIST;
    }
    if(!checkValidationTribeOrAreaName(tribe_name)){
        FREE_TEMP_RESOURCES(str_id,NULL,NULL);
        return ELECTION_INVALID_NAME;
    }
    MapResult result = mapPut(election->tribes,str_id,tribe_name);
    FREE_TEMP_RESOURCES(str_id,NULL,NULL);
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
    FREE_TEMP_RESOURCES(str_id,NULL,NULL);
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
    if (areas_to_remove == NULL) {
        DESTROY_AND_RETURN_ELECTION(election);
    }
    int areas_to_remove_counter = 0;
    MAP_FOREACH(iter, election->areas) {
        int area_id = convertStringToInt(iter);
        SSCANF_CHECK_AND_FREE(area_id, areas_to_remove, ELECTION_ERROR);
        if (should_delete_area(area_id)) { //checking which areas need to be removed
            assert(area_id>=0);
            areas_to_remove[areas_to_remove_counter] = iter; //insert them to the array
            areas_to_remove_counter++;
        }
    }
    for (int i=0; i<areas_to_remove_counter; i++) {//looping through the array and removing
        int area_id_to_remove = convertStringToInt(areas_to_remove[i]);
        SSCANF_CHECK_AND_FREE(area_id_to_remove, areas_to_remove, ELECTION_ERROR);
        if(mapIdListRemove(election->votes, area_id_to_remove) != MAP_ID_LIST_SUCCESS) { //no need to check the id
            return ELECTION_NULL_ARGUMENT;
        }
        if(mapRemove(election->areas, areas_to_remove[i]) != MAP_SUCCESS){ //no need to check if the key exists
            return ELECTION_NULL_ARGUMENT;
        }
    }
    FREE_TEMP_RESOURCES(areas_to_remove,NULL,NULL);
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
    if (mapGetSize(election->areas) == 0 || mapGetSize(election->tribes) == 0) { // no tribes or areas
        return areas_to_tribes_mapping;
    }
    if(!electionComputeAreasToTribesMappingAux(election,areas_to_tribes_mapping)) { //no tribes in the map
        return NULL;
    }
    return areas_to_tribes_mapping;
}