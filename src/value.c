#include "../internals/arena.h"
#include "../internals/array.h"
#include "../internals/error.h"
#include "../internals/obj.h"
#include "../internals/parse.h"
#include "../internals/types.h"
#include "../internals/value.h"


extern Arena *allocator;


JSON *make_value(char *data,
                 size_t *offset)
{
    char c = data[*offset];

    Hint hint = determine_type(c);
    Type type;

    if (hint == HINT_STRING)
    {
        (*offset)++;

        type.s = NULL;

        handle_string(&type.s,
                      data,
                      offset);
    }
    else if (hint == HINT_DIGIT)
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
    else if (hint == HINT_ARRAY)
    {
        (*offset)++;

        type.a = make_array(data,
                            offset);
    }
    else if (hint == HINT_OBJECT)
    {
        (*offset)++;

        type.o = make_object(data,
                             offset);
    }
    else if (hint == HINT_NULL)
    {
        hint = hint;
        type.n = handle_null(data,
                             offset);
    }
    else
    {
        throw(__FILE__,
              __FUNCTION__,
              __LINE__,
              "Error hint type: %d",
              hint);
    }

    JSON *json = alloc_arena(allocator,
                             sizeof(JSON));

    json->hint = hint;
    json->type = type;

    return json;
}
