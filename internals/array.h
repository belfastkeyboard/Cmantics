#pragma once


#include "../internals/arena.h"
#include "../json.h"


typedef struct Array
{
    Value **values;
    size_t nmemb;
    size_t capacity;
} Array;


Array *create_array(Arena *arena);


Array *make_array(JSON *json);

void destroy_array(Array *array);


Array *parse_array(JSON *json,
                   char *data,
                   size_t *offset);


void push_array(Array *array,
                Value *value);

void pop_array(Array *array,
               size_t index);
