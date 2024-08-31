#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

typedef struct Pair Pair;
typedef struct JSON JSON;
typedef enum value_t value_t;
typedef union json_t json_t;

typedef struct Object
{
    size_t pair_size;
    Pair *kv_pairs;
} Object;
typedef struct Array
{
    size_t size;
    value_t *hints;
    json_t  *values;
} Array;


typedef enum value_t
{
    VALUE_ERROR,
    VALUE_NULL,
    VALUE_STRING,
    VALUE_DIGIT,
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_BOOL,
    VALUE_OBJECT,
    VALUE_ARRAY
} value_t;
typedef union json_t
{
    void  *n;
    char  *s;
    int    i;
    float  f;
    bool   b;
    Object o;
    Array  a;
} json_t;

typedef struct Value
{
    value_t value_enum;
    json_t type;
} Value;

JSON *json_fopen(const char *file);
void json_fclose(JSON *json);
Value *json_find(JSON *json, const char *key);
