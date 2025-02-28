#include "../../internals/arena.h"
#include "../../internals/dict.h"
#include "../../internals/ini/section.h"


struct Section *create_header(const char *name,
                              Arena *arena)
{
    struct Section *header = alloc_arena(arena,
                                         sizeof(struct Section));

    header->name = name;
    header->pairs = create_dict(arena,
                                false);

    return header;
}
