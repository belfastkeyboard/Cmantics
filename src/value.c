#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "../internals/arena.h"
#include "../internals/array.h"
#include "../internals/error.h"
#include "../internals/obj.h"
#include "../internals/value.h"
#include "../internals/str.h"


static Hint determine_type(const char c,
                           char *data,
                           const size_t offset)
{
    Hint hint;

    if (c == '"')
    {
        hint = JSON_STRING;
    }
    else if (isdigit(c) ||
             c == '-')
    {
        hint = JSON_INT;

        size_t len = strspn(data + offset,
                            "+-0123456789.eE");

        for (int i = 0; i < len; i++)
        {
            if (data[offset + i] == '.' ||
                data[offset + i] == 'e' ||
                data[offset + i] == 'E')
            {
                hint = JSON_FLOAT;

                break;
            }
        }
    }
    else if (c == 'f' || c == 't')
    {
        hint = JSON_BOOL;
    }
    else if (c == '[')
    {
        hint = JSON_ARRAY;
    }
    else if (c == '{')
    {
        hint = JSON_OBJECT;
    }
    else if (c == 'n')
    {
        hint = JSON_NULL;
    }
    else
    {
        hint = JSON_ERROR;
    }

    return hint;
}


int handle_integer(char *data,
                   size_t *offset)
{
    char *ptr;
    const size_t off = *offset;

    int result = (int)strtol(data + off,
                             &ptr,
                             10);

    *offset += ptr - (data + off);

    return result;
}


double handle_float(char *data,
                    size_t *offset)
{
    char *ptr;
    const size_t off = *offset;

    double result = strtod(data + off,
                           &ptr);

    *offset += ptr - (data + off);

    return result;
}


bool handle_boolean(char *data,
                    const char c,
                    size_t *offset)
{
    char boolean[6] = { 0 };
    bool result;

    size_t len = (c == 'f') ? 5 :
                 4;

    strncpy(boolean,
            data + *offset,
            len);

    *offset += len;

    if (strcmp(boolean,
               "true") == 0)
    {
        result = true;
    }
    else if (strcmp(boolean,
                    "false") == 0)
    {
        result = false;
    }
    else
    {
        throw(__FILE__,
              __FUNCTION__,
              __LINE__,
              "Error boolean type: %s",
              boolean);
    }

    return result;
}


void *handle_null(char *data,
                  size_t *offset)
{
    char null[5] = { 0 };

    size_t len = 4;

    strncpy(null,
            data + *offset,
            len);

    *offset += len;

    return NULL;
}


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


Value *parse_value(char *data,
                   size_t *offset,
                   Array *meta,
                   Arena *arena)
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
                               arena);
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

        type.a = parse_array(data,
                             offset,
                             meta,
                             arena);
    }
    else if (hint == JSON_OBJECT)
    {
        (*offset)++;

        type.o = parse_object(data,
                              offset,
                              meta,
                              arena);
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

    Value *value = make_value(arena,
                              hint,
                              type);

    return value;
}
