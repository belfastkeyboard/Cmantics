#include <assert.h>
#include <string.h>
#include "../internals/eval.h"


enum Eval evaluation(const char *data,
                     size_t *offset)
{
    assert(data);

    enum Eval eval;
    *offset += strcspn(data + *offset,
                       "{}[]\"");

    char c = *(data + *offset);
    switch (c)
    {
        case '{':
            eval = EVAL_MAKE_OBJ;
            break;
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
