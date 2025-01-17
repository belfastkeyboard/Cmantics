#include <string.h>
#include "../internals/obj.h"
#include "../internals/array.h"
#include "../internals/get.h"


JSON *scan_object(const Object *object,
                  const char *key)
{
    JSON *result = NULL;

    for (int i = 0; i < object->pair_size && !result; i++)
    {
        Pair *pair = &object->kv_pairs[i];

        if (strcmp(pair->key,
                   key) == 0)
        {
            result = &pair->value;
        }
    }

    return result;
}

JSON *lookup_array(const Array *array,
                   size_t index)
{
    JSON *result = NULL;

    if (index < array->size)
    {
        result = array->values[index];
    }

    return result;
}
