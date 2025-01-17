#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>


typedef struct Object Object;
typedef struct Array Array;


typedef enum Hint
{
    HINT_ERROR,
    HINT_NULL,
    HINT_STRING,
    HINT_DIGIT,
    HINT_INT,
    HINT_FLOAT,
    HINT_BOOL,
    HINT_OBJECT,
    HINT_ARRAY
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


typedef struct JSON
{
    Hint hint;
    Type type;
} JSON;


void json_open(void);

void json_close(void);


JSON *json_find(JSON *value,
                const char *key);

JSON *json_lookup(JSON *value,
                  size_t index);


JSON *json_make_value(Hint hint,
                      void *type);


JSON *json_make_object(void);

void json_push_object(Object *object,
                      char *key,
                      JSON *value);

void json_pop_object(Object *object,
                     char *key);


JSON *json_make_array(void);

void json_push_array(Array *array,
                     JSON *value);

void json_pop_array(Array *array,
                    size_t index);


__attribute__((warn_unused_result))
JSON *json_read(const char *path);

void json_write(const char *path,
                const JSON *value);
