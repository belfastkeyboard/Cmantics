#pragma once


#include "../json.h"
#include "../internals/dict.h"


struct DictPair parse_pair(char *data,
                           size_t *offset,
                           Array *meta,
                           Arena *arena);
