#include <string.h>
#include "../internals/obj.h"
#include "../internals/array.h"
#include "../internals/get.h"


Value *find_object(const Object *object,
                   const char *key)
{
    return find_dict(object->dict,
                     key);
}

Value *find_array(const Array *array,
                  size_t index)
{
    Value *result = NULL;

    if (index < array->nmemb)
    {
        result = array->values[index];
    }

    return result;
}
