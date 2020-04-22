#include "keyValue.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

struct KeyValue_t {
    char* key;
    char* value;
};

KeyValue keyValueCreate() {
    KeyValue keyValue = malloc(sizeof(*keyValue));
    if (keyValue == NULL) {
        return NULL;
    }
    return keyValue;
}

void keyValueDestroy(KeyValue keyValue) {
    if (keyValue == NULL) {
        return;
    }
    if (keyValue->value != NULL) {
        free(keyValue->value);
    }
    if (keyValue->key != NULL){
        free(keyValue->key);
    }
    free(keyValue);
}

char* keyGet(KeyValue keyValue) {
    if (keyValue == NULL || keyValue->key == NULL) {
        return NULL;
    }
    return keyValue->key;
}

char* valueGet(KeyValue keyValue) {
    if (keyValue == NULL || keyValue->value == NULL) {
        return NULL;
    }
    return keyValue->value;
}

KeyValueResult keySet(KeyValue keyValue,const char* key) {
    if (keyValue == NULL || key == NULL) {
        return KEY_VALUE_NULL_ARGUMENT;
    }
    ALLOCATE(key); //allocates space and copying the string inside
    return KEY_VALUE_SUCCESS;
}

KeyValueResult valueSet(KeyValue keyValue, const char* value) {
    if (keyValue == NULL || value == NULL) {
        return KEY_VALUE_NULL_ARGUMENT;
    }
    ALLOCATE(value); //allocates space and copying the string inside
    return KEY_VALUE_SUCCESS;
}
