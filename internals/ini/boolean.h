#pragma once

#include <stdbool.h>
#include "../../ini.h"
#include "../arena.h"


Boolean make_boolean(bool boolean,
                     const char *string,
                     Arena *arena);
