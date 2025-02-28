#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "internals/boolean.h"


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
    void    *n;
    char    *s;
    long    i;
    double  f;
    Boolean b;
    Object  *o;
    Array   *a;
} TypeJSON;

typedef struct Value
{
    HintJSON hint;
    TypeJSON type;
} ValueJSON;


JSON *create_json(void);

void destroy_json(JSON **json);


ValueJSON *get_json(JSON *json);

ValueJSON *lookup_json(ValueJSON *object,
                       const char *key);

ValueJSON *scan_json(ValueJSON *array,
                     size_t index);


ValueJSON *make_json(JSON* json,
                     HintJSON hint);


void push_json(ValueJSON *array,
               ValueJSON *value);

void pop_json(ValueJSON *array,
              size_t index);


void insert_json(ValueJSON *object,
                 const char *key,
                 ValueJSON *value);

void erase_json(ValueJSON *object,
                const char *key);


size_t count_json(ValueJSON* container);


void parse_json(JSON *json,
                const char *path);

void write_json(const JSON *value,
                const char *path);
