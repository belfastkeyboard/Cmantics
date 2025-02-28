#pragma once


#include "../json.h"
#include "../internals/dict.h"


struct DictPair parse_pair(JSON *json,
                           char *data,
                           size_t *offset);
