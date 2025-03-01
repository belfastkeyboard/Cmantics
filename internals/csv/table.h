#pragma once

#include "../../csv.h"
#include "value.h"
#include <stddef.h>


struct Table
{
    size_t columns;
    size_t c_capacity;
    size_t rows;
    size_t r_capacity;
    ValueCSV ***table;
};
