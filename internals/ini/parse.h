#pragma once

#include <stdio.h>
#include "../arena.h"
#include "value.h"


struct Section *parse_section(FILE *file,
                              char *line,
                              Array *meta,
                              Arena *arena);


ValueINI *parse_value_ini(char *line,
                          Array *meta,
                          Arena *arena);
