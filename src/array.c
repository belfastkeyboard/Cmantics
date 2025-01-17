#include <string.h>
#include "../internals/types.h"
#include "../internals/parse.h"
#include "../internals/obj.h"
#include "../internals/array.h"


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


Array *make_array(Arena *arena,
                  char *data,
                  size_t *offset)
{
    Array *array = alloc_arena(arena,
                               sizeof(Array));

    array->size = count_array(data,
                              *offset);

    array->values = alloc_arena(arena,
                                sizeof(JSON) * array->size);

    *offset += strspn(data + *offset,
                      " \n");

    for (int i = 0; i < array->size; i++)
    {
        Type type;
        char c = data[*offset];

        Hint hint = determine_type(c);

        if (hint == HINT_DIGIT)
        {
            if (determine_digit(data,
                                *offset) == HINT_INT)
            {
                hint = HINT_INT;

                type.i = handle_integer(data,
                                         offset);
            }
            else
            {
                hint = HINT_FLOAT;

                type.f = handle_float(data,
                                       offset);
            }
        }
        else if (hint == HINT_BOOL)
        {
            type.b = handle_boolean(data,
                                     c,
                                     offset);
        }
        else if (hint == HINT_STRING)
        {
            (*offset)++;

            type.s = NULL;

            handle_string(arena,
                          &type.s,
                          data,
                          offset);
        }
        else if (hint == HINT_OBJECT)
        {
            (*offset)++;

            type.o = make_object(arena,
                                  data,
                                  offset);
        }
        else if (hint == HINT_ARRAY)
        {
            (*offset)++;

            type.a = make_array(arena,
                                 data,
                                 offset);
        }
        else
        {
            type.n = handle_null(data,
                                  offset);
        }

        JSON *value = alloc_arena(arena,
                                   sizeof(JSON));

        value->hint = hint;
        value->type = type;

        array->values[i] = value;

        *offset += strspn(data + *offset,
                          ", \n");
    }

    *offset += strspn(data + *offset,
                      "]") + 1;

    return array;
}
