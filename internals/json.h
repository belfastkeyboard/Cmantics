#pragma onec


#include "arena.h"
#include "array.h"


typedef struct JSON
{
    Arena *arena;
    Array *meta;
    Value *value;
} JSON;
