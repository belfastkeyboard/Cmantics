#pragma once

#include "../arena.h"
#include "../../json.h"


ValueJSON *make_value_json(Arena *arena,
                           const HintJSON hint,
                           const TypeJSON type);
