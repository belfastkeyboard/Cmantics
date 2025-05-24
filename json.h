/**
 * @file json.h
 * @brief JSON Parser
 *
 * Provides utilities for reading, editing and writing JSON.
 *
 * @author Riain Ó Tuathail
 * @date 2025-05-24
 * @version v1.1
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>


typedef struct Object Object;
typedef struct Array Array;
typedef struct JSON JSON;


typedef enum HintJSON
{
    JSON_INT,
    JSON_FLOAT,
    JSON_BOOL,
    JSON_STRING,
    JSON_NULL,
    JSON_ARRAY,
    JSON_OBJECT,
    JSON_ERROR
} HintJSON;

typedef union TypeJSON
{
    void   *n;
    char   *s;
    long    i;
    double  f;
    bool    b;
    Object *o;
    Array  *a;
} TypeJSON;

typedef struct Value
{
    HintJSON hint;
    TypeJSON type;
} ValueJSON;


/// Allocate a JSON parser that must be freed with destroy_json()
__attribute__((warn_unused_result))
JSON *create_json(void);

/// Destroy an allocated JSON parser, gracefully handles NULL JSON pointers
void destroy_json(JSON **json);


/// Parse a JSON file
void parse_json(JSON *json,
                const char *path);

/// Output contents of JSON parser to file
void write_json(const JSON *value,
                const char *path);


/// Retrieve an editable value from the opaque JSON parser, returns NULL on failure
ValueJSON *get_json(JSON *json);

/// Retrieve a value from a JSON Object, returns NULL on failure
ValueJSON *lookup_json(ValueJSON *object,
                       const char *key);

/// Retrieve a value from a JSON Array, returns NULL on failure
ValueJSON *scan_json(ValueJSON *array,
                     size_t index);


/// Allocate a value with the JSON parser's allocator, returns default value
ValueJSON *make_json(JSON* json,
                     HintJSON hint);


/// Insert a value into a JSON Object
void insert_json(ValueJSON *object,
                 const char *key,
                 ValueJSON *value);

/// Erase a value from a JSON object
void erase_json(ValueJSON *object,
                const char *key);


/// Push back a value into a JSON Array
void push_json(ValueJSON *array,
               ValueJSON *value);

/// Pop a value at the index of a JSON Array
void pop_json(ValueJSON *array,
              size_t index);


/// Get the number of values in a JSON Object or Array, returns SIZE_MAX on failure
size_t count_json(ValueJSON* container);
