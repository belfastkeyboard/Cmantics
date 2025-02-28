#pragma once


#include "arena.h"


char *make_string(const char *src,
                  Arena *arena);


char *handle_string(char *data,
                    size_t *offset,
                    Arena *arena);
