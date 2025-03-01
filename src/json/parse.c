#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "../../internals/error.h"
#include "../../internals/json/obj.h"
#include "../../internals/json/parse.h"


enum Eval
{
    EVAL_ERROR,
    EVAL_FIN_OBJ,
    EVAL_MAKE_PAIR
};


static struct DictPair parse_pair(char *data,
                                  size_t *offset,
                                  Array *meta,
                                  Arena *arena);


static enum Eval evaluation(const char *data,
                            size_t *offset)
{
    enum Eval eval;
    *offset += strcspn(data + *offset,
                       "{}[]\"");

    char c = *(data + *offset);
    switch (c)
    {
        case '}':
            eval = EVAL_FIN_OBJ;
            break;
        case '"':
            eval = EVAL_MAKE_PAIR;
            break;
        default:
            eval = EVAL_ERROR;
            break;
    }

    (*offset)++;

    return eval;
}


static HintJSON determine_type(const char c,
                               char *data,
                               const size_t offset)
{
    HintJSON hint;

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


static long handle_integer(char *data,
                           size_t *offset)
{
    char *ptr;
    const size_t off = *offset;

    long result = strtol(data + off,
                         &ptr,
                         10);

    *offset += ptr - (data + off);

    return result;
}


static double handle_float(char *data,
                           size_t *offset)
{
    char *ptr;
    const size_t off = *offset;

    double result = strtod(data + off,
                           &ptr);

    *offset += ptr - (data + off);

    return result;
}


static bool handle_boolean(char *data,
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


static void *handle_null(char *data,
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

static char *handle_string(char *data,
                           size_t *offset,
                           Arena *arena)
{
    const size_t len = strcspn(data + *offset,
                               "\"");

    char *string = calloc_arena(arena,
                                len + 1);

    strncpy(string,
            data + *offset,
            len);

    *offset += len + 1;

    return string;
}


static Object *parse_object(char *data,
                            size_t *offset,
                            Array *meta,
                            Arena *arena)
{

    Object *object = make_object(meta,
                                 arena);

    while (evaluation(data,
                      offset) != EVAL_FIN_OBJ)
    {
        struct DictPair pair = parse_pair(data,
                                          offset,
                                          meta,
                                          arena);

        insert_dict(object->dict,
                    pair.key,
                    pair.value);
    }

    return object;
}


static Array *parse_array(char *data,
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
        ValueJSON *value = parse_value_json(data,
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


static struct DictPair parse_pair(char *data,
                                  size_t *offset,
                                  Array *meta,
                                  Arena *arena)
{
    const char *key = handle_string(data,
                                    offset,
                                    arena);

    *offset += strspn(data + *offset,
                      ": ");

    void *value = parse_value_json(data,
                                   offset,
                                   meta,
                                   arena);

    struct DictPair pair = {
        .key = key,
        .value = value
    };

    return pair;
}


ValueJSON *parse_value_json(char *data,
                            size_t *offset,
                            Array *meta,
                            Arena *arena)
{
    char c = data[*offset];

    HintJSON hint = determine_type(c,
                                   data,
                                   *offset);

    TypeJSON type;

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

    ValueJSON *value = make_value_json(arena,
                                       hint,
                                       type);

    return value;
}
