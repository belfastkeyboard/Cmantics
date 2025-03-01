#include "../../internals/arena.h"
#include "../../internals/dict.h"
#include "../../internals/ini/section.h"


struct Section *create_section(const char *name,
                               Arena *arena)
{
    struct Section *section = alloc_arena(arena,
                                         sizeof(struct Section));

    section->name = name;
    section->pairs = create_dict(arena,
                                false);

    return section;
}
