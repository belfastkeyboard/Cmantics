#pragma once


#include "../internals/pair.h"
#include "../json.h"


Value *find_object(const Object *object,
                   const char *key);

Value *find_array(const Array *array,
                  size_t index);
