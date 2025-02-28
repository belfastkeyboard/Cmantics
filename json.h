#pragma once

#include <stdbool.h>
#include <stddef.h>


typedef struct Object Object;
typedef struct Array Array;
typedef struct JSON JSON;


typedef enum Hint
{
    JSON_INT,
    JSON_FLOAT,
    JSON_BOOL,
    JSON_STRING,
    JSON_NULL,
    JSON_ARRAY,
    JSON_OBJECT,
    JSON_ERROR
} Hint;

typedef union Type
{
    void   *n;
    char   *s;
    long    i;
    double  f;
    bool    b;
    Object *o;
    Array  *a;
} Type;

typedef struct Value
{
    Hint hint;
    Type type;
} Value;


JSON *create_json(void);

void destroy_json(JSON **json);


Value *get_json(JSON *json);

Value *lookup_json(Value *object,
                   const char *key);

Value *scan_json(Value *array,
                 size_t index);


Value *make_json(JSON* json,
                 Hint hint);


void push_json(Value *array,
               Value *value);

void pop_json(Value *array,
               size_t index);


void insert_json(Value *object,
                 const char *key,
                 Value *value);

void erase_json(Value *object,
                const char *key);


size_t count_json(Value* container);


//JSON *json_make_object(void);
//
//void json_push_object(Object *object,
//                      char *key,
//                      JSON *value);
//
//void json_pop_object(Object *object,
//                     char *key);
//
//
//JSON *json_make_array(void);
//
//void json_push_array(Array *array,
//                     JSON *value);
//
//void json_pop_array(Array *array,
//                    size_t index);


void parse_json(JSON *json,
                const char *path);

void write_json(const JSON *value,
                const char *path);
