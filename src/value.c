#include "../internals/arena.h"
#include "../internals/array.h"
#include "../internals/error.h"
#include "../internals/obj.h"
#include "../internals/parse.h"
#include "../internals/types.h"
#include "../internals/value.h"
#include "../internals/json.h"


Value *make_value(Arena *arena,
                  const Hint hint,
                  const Type type)
{
    Value *value = alloc_arena(arena,
                               sizeof(Value));

    value->hint = hint;
    value->type = type;

    return value;
}


Value *parse_value(JSON *json,
                   char *data,
                   size_t *offset)
{
    char c = data[*offset];

    Hint hint = determine_type(c,
                               data,
                               *offset);

    Type type;

    if (hint == JSON_STRING)
    {
        (*offset)++;

        type.s = handle_string(data,
                               offset,
                               json->arena);
    }
    else if (hint == JSON_INT)
    {
        type.i = handle_integer(data,
                                offset);
    }
    else if (hint == JSON_FLOAT)
    {
        type.f = handle_float(data,
                              offset);
    }
    else if (hint == JSON_BOOL)
    {
        type.b = handle_boolean(data,
                                c,
                                offset);
    }
    else if (hint == JSON_ARRAY)
    {
        (*offset)++;

        type.a = parse_array(json,
                             data,
                             offset);
    }
    else if (hint == JSON_OBJECT)
    {
        (*offset)++;

        type.o = parse_object(json,
                              data,
                              offset);
    }
    else if (hint == JSON_NULL)
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

    Value *value = make_value(json->arena,
                              hint,
                              type);

    return value;
}
