#ifndef KEY_VALUE_H_
#define KEY_VALUE_H_

#include <stdbool.h>
#include <string.h>
/**
* KeyValue struct
*
* Implements a key-value struct container type.
* The type of the key and the value is string (char *).
* This is only a helper struct for the map implementation.
*
* The following functions are available:
*   keyValueCreate		- Creates a new empty key-value struct
*   keyValueDestroy		- Deletes an existing key-value and frees all resources
*   keyGet  	        - Returns the requested key, NULL if dosent exist.
*   valueGet  	        - Returns the data paired to a key which matches the given key.
*/

/** Type for defining the keyValue */
typedef struct KeyValue_t* KeyValue;

/**
* keyValueCreate: Allocates a new empty key&value.
*
* @return
* 	NULL - if allocations failed.
* 	A new key-value pair in case of success.
*/
KeyValue keyValueCreate();

/**
* keyValueDestroy: Deallocates an existing key-value.
*
* @param keyValue - Target key-value to be deallocated. If the key and the value
*                   are NULL nothing will be done.
*/
void keyValueDestroy(KeyValue keyValue);

/**
*	keyGet: Returns the key corresponds to the entered keyValue, NULL if it dosent exist.
*
* @param key - The key element which need to be found.
* @return
*  NULL if a NULL pointer was sent or if the key does not exist.
* 	A pointer to the key otherwise.
*/
char* keyGet(KeyValue keyValue);

/**
*	valueGet: Returns the value corresponds to the entered keyValue, NULL if it dosent exist.
*
* @param valueElement - The key element which need to be found.
* @return
*  NULL if a NULL pointer was sent or if the value does not exist.
* 	A pointer to the value otherwise.
*/
char* valueGet(KeyValue keyValue);


#endif /* KEY_VALUE_H_ */