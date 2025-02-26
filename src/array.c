#include <string.h>
#include "../internals/array.h"
#include "../internals/obj.h"
#include "../internals/parse.h"
#include "../internals/value.h"


extern Arena *allocator;


static size_t count_array(const char *data,
                          const size_t offset)
{
    size_t count = 0;

    const size_t pos = strspn(data + offset,
                              " \n");

    if (data[offset + pos] != ']')
    {
        size_t depth = 0;
        size_t len = strlen(data);
        bool in_string = false;

        for (int i = 0; i < len; i++)
        {
            char c = data[offset + i];

            if (!in_string &&
                c == ',' &&
                depth == 0)
            {
                count++;
            }
            else if (!in_string &&
                     c == '{' ||
                     c == '[')
            {
                depth++;
            }
            else if (!in_string &&
                     depth > 0 &&
                     (c == '}' ||
                     c == ']'))
            {
                depth--;
            }
            else if (depth == 0 &&
                     c == '"')
            {
                in_string = !in_string;
            }
            else if (!in_string &&
                     depth == 0 &&
                     c == ']')
            {
                break;
            }
        }

        count++;
    }

    return count;
}


Array *make_array(char *data,
                  size_t *offset)
{
    Array *array = alloc_arena(allocator,
                               sizeof(Array));

    array->size = count_array(data,
                              *offset);

    array->values = alloc_arena(allocator,
                                sizeof(JSON) * array->size);

    *offset += strspn(data + *offset,
                      " \n");

    for (int i = 0; i < array->size; i++)
    {
        JSON *json = make_value(data,
                                offset);

        array->values[i] = json;

        *offset += strspn(data + *offset,
                          ", \n");
    }

    *offset += strspn(data + *offset,
                      "]") + 1;

    return array;
}
