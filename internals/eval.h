#pragma once

#include <stddef.h>


enum Eval
{
    EVAL_ERROR,
    EVAL_MAKE_OBJ,
    EVAL_FIN_OBJ,
    EVAL_MAKE_PAIR
};


enum Eval evaluation(const char *data,
                     size_t *offset);
