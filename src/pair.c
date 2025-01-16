#include <string.h>
#include "../internals/arena.h"
#include "../internals/parse.h"
#include "../internals/types.h"
#include "../internals/error.h"
#include "../internals/array.h"
#include "../internals/obj.h"


Pair make_pair(Arena *arena,
               char *data,
               size_t *offset)
{
    Pair pair = { 0 };

    handle_string(arena,
                  &pair.key,
                  data,
                  offset);

    *offset += strspn(data + *offset,
                      ": ");

    char c = data[*offset];

    Hint hint = determine_type(c);

    if (hint == HINT_STRING)
    {
        (*offset)++;

        pair.value.type.s = NULL;

        handle_string(arena,
                      &pair.value.type.s,
                      data,
                      offset);
    }
    else if (hint == HINT_DIGIT)
    {
        if (determine_digit(data,
                            *offset) == HINT_INT)
        {
            hint = HINT_INT;
            pair.value.type.i = handle_integer(data,
                                               offset);
        }
        else
        {
            hint = HINT_FLOAT;
            pair.value.type.f = handle_float(data,
                                             offset);
        }
    }
    else if (hint == HINT_BOOL)
    {
        pair.value.type.b = handle_boolean(data,
                                           c,
                                           offset);
    }
    else if (hint == HINT_ARRAY)
    {
        (*offset)++;

        pair.value.type.a = make_array(arena,
                                       data,
                                       offset);
    }
    else if (hint == HINT_OBJECT)
    {
        (*offset)++;
        pair.value.type.o = make_object(arena,
                                        data,
                                        offset);
    }
    else if (hint == HINT_NULL)
    {
        pair.value.hint = hint;
        pair.value.type.n = handle_null(data,
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

    pair.value.hint = hint;

    return pair;
}
