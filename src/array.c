#include <malloc.h>
#include <string.h>
#include "../internals/array.h"
#include "../internals/obj.h"
#include "../internals/value.h"


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

    Hint hint = JSON_ARRAY;
    Type type = {
        .a = array
    };

    Value *value = make_value(arena,
                              hint,
                              type);

    push_array(meta,
               value);

    return array;
}


Array *parse_array(char *data,
                   size_t *offset,
                   Array *meta,
                   Arena *arena)
{
    Array *array = make_array(meta,
                              arena);

    *offset += strspn(data + *offset,
                      " \n");

    while (data[*offset] != ']')
    {
        Value *value = parse_value(data,
                                   offset,
                                   meta,
                                   arena);

        push_array(array,
                   value);

        *offset += strspn(data + *offset,
                          ", \n");
    }

    *offset += strspn(data + *offset,
                      "]") + 1;

    return array;
}


void resize_array(Array *array)
{
    const size_t new_cap = (array->capacity) ? array->capacity * 2 :
                                               1;

    Value **values = realloc(array->values,
                             new_cap * sizeof(Value));

    if (values)
    {
        array->values = values;
        array->capacity = new_cap;
    }
}

void push_array(Array *array,
                Value *value)
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
        array->nmemb--;

        Value *last = array->values[array->nmemb];

        array->values[index] = last;
    }
}
