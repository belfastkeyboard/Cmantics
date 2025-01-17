#pragma once

#include "../json.h"


typedef char *Key;


typedef struct Pair
{
    Key key;
    JSON value;
} Pair;


Pair make_pair(Arena *arena,
               char *data,
               size_t *offset);
