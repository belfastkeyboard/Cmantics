#pragma once

#include "../array.h"
#include "value.h"


ValueJSON *parse_value_json(char *data,
                            size_t *offset,
                            Array *meta,
                            Arena *arena);
