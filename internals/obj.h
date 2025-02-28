#pragma once

#include <stddef.h>
#include "arena.h"
#include "dict.h"
#include "pair.h"


typedef struct Object {
    struct Dict *dict;
} Object;


Object *make_object(JSON *json);


Object *parse_object(JSON *json,
                     char *data,
                     size_t *offset);
