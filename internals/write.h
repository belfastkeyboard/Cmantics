#pragma once

#include "obj.h"


void write_value(FILE *file,
                 const Value *value,
                 int indentation,
                 Arena *arena);
