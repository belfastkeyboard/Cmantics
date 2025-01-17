#pragma once

#include <stdbool.h>
#include <stddef.h>


void handle_string(char **src,
                   char *data,
                   size_t *offset);


int handle_integer(char *data,
                   size_t *offset);


double handle_float(char *data,
                    size_t *offset);


bool handle_boolean(char *data,
                    char c,
                    size_t *offset);


void *handle_null(char *data,
                  size_t *offset);
