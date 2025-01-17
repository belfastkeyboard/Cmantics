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


typedef struct Value
{
    Hint hint;
    Type type;
} Value;


__attribute__((warn_unused_result))
Value *json_open(const char *path);

void json_close(void);

Value *json_find(Value *value,
                 const char *key);

Value *json_lookup(Value *value,
                   size_t index);


void json_write(const char *path,
                const Value *value);
