#pragma once

#include "../arena.h"
#include "../../ini.h"


ValueINI *make_value_ini(Arena *arena,
                         HintINI hint,
                         TypeINI type);
