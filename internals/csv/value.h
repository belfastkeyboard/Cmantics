#pragma once

#include "../arena.h"
#include "../../csv.h"


ValueCSV *make_value_csv(Arena *arena,
                         HintCSV hint,
                         TypeCSV type);
