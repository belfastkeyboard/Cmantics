#pragma once

#include "../arena.h"


struct Section
{
    const char *name;
    struct Dict *pairs;
};

struct Section *create_section(const char *name,
                               Arena *arena);
