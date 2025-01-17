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


__attribute__((warn_unused_result))
JSON *json_open(const char *path);

void json_close(void);

JSON *json_find(JSON *value,
                const char *key);

JSON *json_lookup(JSON *value,
                  size_t index);


void json_write(const char *path,
                const JSON *value);
