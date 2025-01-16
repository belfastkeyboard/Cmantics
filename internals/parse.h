#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "arena.h"


void handle_string(Arena *arena,
                   char **src,
                   char *data,
                   size_t *offset);


int handle_integer(char *data,
                   size_t *offset);


float handle_float(char *data,
                   size_t *offset);


bool handle_boolean(char *data,
                    char c,
                    size_t *offset);


void *handle_null(char *data,
                  size_t *offset);
