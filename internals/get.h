#pragma once

#include "../json.h"


JSON *scan_object(const Object *object,
                  const char *key);

JSON *lookup_array(const Array *array,
                   size_t index);
