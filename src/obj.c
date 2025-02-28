#include <string.h>
#include "../internals/arena.h"
#include "../internals/pair.h"
#include "../internals/eval.h"
#include "../internals/parse.h"
#include "../internals/obj.h"
#include "../internals/json.h"
#include "../internals/value.h"


static size_t count_children(char *data,
                             const size_t *offset)
{
    char *cpy = data + *offset;
    size_t count = 0;

    size_t pos = strspn(cpy,
                        " \n");

    if (cpy[pos] != '}')
    {
        size_t depth = 0;
        size_t len = strlen(cpy);
        bool in_string = false;

        for (size_t i = pos; i < len; i++)
        {
            char c = cpy[i];

            if (!in_string &&
                c == ':' &&
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
                     c == '}')
            {
                break;
            }
        }
    }

    return count;
}


Object *make_object(JSON *json)
{
    Object *object = alloc_arena(json->arena,
                                 sizeof(Object));

    object->dict = create_dict(json->arena);

    Hint hint = JSON_OBJECT;
    Type type = {
        .o = object
    };

    Value *value = make_value(json->arena,
                              hint,
                              type);

    push_array(json->meta,
               value);

    return object;
}


Object *parse_object(JSON *json,
                     char *data,
                     size_t *offset)
{

    Object *object = make_object(json);

    while (evaluation(data,
                      offset) != EVAL_FIN_OBJ)
    {
        struct DictPair pair = parse_pair(json,
                                          data,
                                          offset);

        insert_dict(object->dict,
                    pair.key,
                    pair.value);
    }

    return object;
}
