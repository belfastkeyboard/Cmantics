#pragma once

#include "../json.h"


Value *scan_object(const Object *object,
                   const char *key);

Value *lookup_array(const Array *array,
                    size_t index);
