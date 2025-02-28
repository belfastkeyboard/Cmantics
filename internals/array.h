#pragma once


#include "../internals/arena.h"
#include "../json.h"


typedef struct Array
{
    void **values;
    size_t size;
    size_t nmemb;
    size_t capacity;
} Array;


Array *create_array(Arena *arena);

void destroy_array(Array *array);


Array *make_array(Array *meta,
                  Arena *arena);


void push_array(Array *array,
                void *value);

void pop_array(Array *array,
               size_t index);
