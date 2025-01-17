#include <string.h>
#include "../internals/arena.h"
#include "../internals/pair.h"
#include "../internals/eval.h"
#include "../internals/parse.h"
#include "../internals/array.h"
#include "../internals/obj.h"


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


Object *make_object(Arena *arena,
                    char *data,
                    size_t *offset)
{
    Object *object = alloc_arena(arena,
                                 sizeof(Object));

    size_t child_count = count_children(data,
                                        offset);

    object->kv_pairs = alloc_arena(arena,
                                   sizeof(Pair) * child_count);

    while (evaluation(data,
                      offset) != EVAL_FIN_OBJ)
    {
        Pair pair = make_pair(arena,
                              data,
                              offset);

        object->kv_pairs[object->pair_size] = pair;

        object->pair_size++;
    }

    return object;
}
