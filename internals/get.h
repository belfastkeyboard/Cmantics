#pragma once

#include "../internals/pair.h"
#include "../json.h"


Pair *scan_object(const Object *object,
                  const char *key,
                  size_t *index);

JSON *lookup_array(const Array *array,
                   size_t index);
