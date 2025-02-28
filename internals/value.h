#pragma once

#include "../json.h"


Value *make_value(Arena *arena,
                  Hint hint,
                  Type type);


Value *parse_value(char *data,
                   size_t *offset,
                   Array *meta,
                   Arena *arena);
