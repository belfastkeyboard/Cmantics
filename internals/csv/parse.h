#pragma once

#include "../arena.h"
#include "table.h"


void parse_rows(FILE *file,
                struct Table *table,
                Arena *arena);
