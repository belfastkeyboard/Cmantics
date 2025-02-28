#include <string.h>
#include "../internals/arena.h"
#include "../internals/array.h"
#include "../internals/pair.h"
#include "../internals/obj.h"
#include "../internals/value.h"


enum Eval
{
    EVAL_ERROR,
    EVAL_FIN_OBJ,
    EVAL_MAKE_PAIR
};


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


Object *make_object(Array *meta,
                    Arena *arena)
{
    Object *object = alloc_arena(arena,
                                 sizeof(Object));

    object->dict = create_dict(arena);

    Hint hint = JSON_OBJECT;
    Type type = {
        .o = object
    };

    Value *value = make_value(arena,
                              hint,
                              type);

    push_array(meta,
               value);

    return object;
}


Object *parse_object(char *data,
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
