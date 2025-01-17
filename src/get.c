#include <string.h>
#include "../internals/obj.h"
#include "../internals/array.h"
#include "../internals/get.h"


Pair *scan_object(const Object *object,
                  const char *key,
                  size_t *index)
{
    Pair *result = NULL;

    for (int i = 0; i < object->size && !result; i++)
    {
        Pair *pair = object->pairs[i];

        if (strcmp(pair->key,
                   key) == 0)
        {
            result = pair;
            *index = i;
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
