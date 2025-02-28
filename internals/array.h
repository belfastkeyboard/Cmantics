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

void destroy_array(Array *array);


Array *make_array(Array *meta,
                  Arena *arena);


Array *parse_array(char *data,
                   size_t *offset,
                   Array *meta,
                   Arena *arena);


void push_array(Array *array,
                Value *value);

void pop_array(Array *array,
               size_t index);
