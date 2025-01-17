#pragma once

#include "obj.h"


void write_array(FILE *file,
                 const Array *array,
                 int indentation);


void write_object(FILE *file,
                  const Object *object,
                  int indentation);
