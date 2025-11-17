/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: json_parser_unchecked.c
 *
 * This case demonstrates violations where JSON parsing functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Simple JSON value types */
typedef enum {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} JsonType;

/* Simple JSON value structure */
typedef struct JsonValue {
    JsonType type;
    union {
        int boolean;
        double number;
        char *string;
        struct JsonValue **array;
        struct JsonKeyValue *object;
    } value;
    size_t count;  /* For arrays and objects */
} JsonValue;

typedef struct JsonKeyValue {
    char *key;
    JsonValue *value;
} JsonKeyValue;

/* NON-COMPLIANT: No validation of JSON string */
JsonValue *parse_json(const char *json_string) {
    /* No validation of json_string */
    JsonValue *value = malloc(sizeof(JsonValue));

    if (*json_string == '{') {  /* Could dereference NULL */
        value->type = JSON_OBJECT;
    } else if (*json_string == '[') {
        value->type = JSON_ARRAY;
    } else {
        value->type = JSON_STRING;
        value->value.string = malloc(strlen(json_string) + 1);  /* json_string could be NULL */
        strcpy(value->value.string, json_string);
    }

    return value;
}

/* NON-COMPLIANT: No validation of JSON value or key */
JsonValue *get_object_value(JsonValue *json_obj, const char *key) {
    /* No validation of json_obj or key */
    if (json_obj->type != JSON_OBJECT) {  /* json_obj could be NULL */
        return NULL;
    }

    for (size_t i = 0; i < json_obj->count; i++) {
        if (strcmp(json_obj->value.object[i].key, key) == 0) {  /* key could be NULL */
            return json_obj->value.object[i].value;
        }
    }

    return NULL;
}

/* NON-COMPLIANT: No validation of array or index */
JsonValue *get_array_element(JsonValue *json_array, size_t index) {
    /* No validation of json_array or index bounds */
    return json_array->value.array[index];  /* json_array could be NULL, index unchecked */
}

/* NON-COMPLIANT: No validation of value extraction */
const char *get_string_value(JsonValue *json_value) {
    /* No validation of json_value or type */
    return json_value->value.string;  /* json_value could be NULL or wrong type */
}

/* NON-COMPLIANT: No validation of numeric conversion */
double get_number_value(JsonValue *json_value) {
    /* No validation of json_value or type */
    return json_value->value.number;  /* json_value could be NULL or wrong type */
}

/* NON-COMPLIANT: No validation of object creation parameters */
JsonValue *create_json_object(JsonKeyValue *key_values, size_t count) {
    JsonValue *obj = malloc(sizeof(JsonValue));
    obj->type = JSON_OBJECT;
    obj->count = count;

    /* No validation of key_values */
    obj->value.object = malloc(count * sizeof(JsonKeyValue));
    memcpy(obj->value.object, key_values, count * sizeof(JsonKeyValue));  /* key_values could be NULL */

    return obj;
}

/* NON-COMPLIANT: No validation of array creation parameters */
JsonValue *create_json_array(JsonValue **elements, size_t count) {
    JsonValue *array = malloc(sizeof(JsonValue));
    array->type = JSON_ARRAY;
    array->count = count;

    /* No validation of elements */
    array->value.array = malloc(count * sizeof(JsonValue *));
    memcpy(array->value.array, elements, count * sizeof(JsonValue *));  /* elements could be NULL */

    return array;
}

/* NON-COMPLIANT: No validation of serialization parameters */
char *serialize_json(JsonValue *json_value) {
    /* No validation of json_value */
    char *result = malloc(1024);  /* Fixed size without checking content */

    switch (json_value->type) {  /* json_value could be NULL */
        case JSON_STRING:
            sprintf(result, "\"%s\"", json_value->value.string);
            break;
        case JSON_NUMBER:
            sprintf(result, "%.2f", json_value->value.number);
            break;
        case JSON_BOOL:
            sprintf(result, "%s", json_value->value.boolean ? "true" : "false");
            break;
        default:
            strcpy(result, "{}");
    }

    return result;
}

/* NON-COMPLIANT: No validation of path traversal */
JsonValue *get_nested_value(JsonValue *root, const char *path) {
    /* No validation of root or path */
    char *path_copy = malloc(strlen(path) + 1);  /* path could be NULL */
    strcpy(path_copy, path);

    JsonValue *current = root;
    char *token = strtok(path_copy, ".");

    while (token && current) {  /* current could become NULL during traversal */
        current = get_object_value(current, token);  /* No error handling */
        token = strtok(NULL, ".");
    }

    free(path_copy);
    return current;
}

int main(void) {
    char *null_json = NULL;
    JsonValue *null_value = NULL;
    char *null_key = NULL;

    /* Examples of dangerous JSON operations */
    // parse_json(null_json);  /* NULL JSON string */
    // get_object_value(null_value, null_key);  /* NULL parameters */
    // get_array_element(null_value, 100);  /* NULL array and out of bounds */
    // get_string_value(null_value);  /* NULL value */
    // get_number_value(null_value);  /* NULL value */
    // create_json_object(NULL, 10);  /* NULL key-value array */
    // create_json_array(NULL, 5);  /* NULL elements array */
    // serialize_json(null_value);  /* NULL value */
    // get_nested_value(null_value, null_key);  /* NULL parameters */

    printf("JSON functions compiled but lack parameter validation\n");
    return 0;
}