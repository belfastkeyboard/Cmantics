#include <malloc.h>
#include <memory.h>
#include "../internals/json/obj.h"
#include "../internals/json/value.h"


Array *create_array(Arena *arena)
{
    Array *array = calloc_arena(arena,
                                sizeof(Array));

    return array;
}

void destroy_array(Array *array)
{
    free(array->values);
}


Array *make_array(Array *meta,
                  Arena *arena)
{
    Array *array = create_array(arena);

    HintJSON hint = JSON_ARRAY;
    TypeJSON type = {
        .a = array
    };

    ValueJSON *value = make_value_json(arena,
                                       hint,
                                       type);

    push_array(meta,
               value);

    return array;
}


void resize_array(Array *array)
{
    const size_t new_cap = (array->capacity) ? array->capacity * 2 :
                                               1;

    void **values = realloc(array->values,
                            new_cap * sizeof(void*));

    if (values)
    {
        array->values = values;
        array->capacity = new_cap;
    }
}

void push_array(Array *array,
                void *value)
{
    if (array->nmemb >= array->capacity)
    {
        resize_array(array);
    }

    array->values[array->nmemb] = value;
    array->nmemb++;
}

void pop_array(Array *array,
               const size_t index)
{
    if (array->nmemb &&
        index < array->nmemb)
    {
        memmove(&array->values[index],
                &array->values[index + 1],
                (array->nmemb - index - 1) * sizeof(void *));

        array->nmemb--;
    }
}
