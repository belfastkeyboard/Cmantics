#pragma once

#include <stdio.h>
#include "../../internals/csv/table.h"


void write_values(FILE *file,
                  const struct Table *table);
