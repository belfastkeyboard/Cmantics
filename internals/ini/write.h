#pragma once

#include <stdio.h>
#include "../dict.h"


void write_section(FILE *file,
                   struct DictPair section,
                   Arena *arena);
