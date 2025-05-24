#include "../../internals/arena.h"
#include "../../internals/ini/value.h"


ValueINI *make_value_ini(Arena *arena,
                         HintINI hint,
                         TypeINI type)
{
    ValueINI *value = alloc_cmantics_arena(arena,
                                           sizeof(ValueINI));

    value->hint = hint;
    value->type = type;

    return value;
}
