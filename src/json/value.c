#include "../../internals/arena.h"
#include "../../internals/json/value.h"


ValueJSON *make_value_json(Arena *arena,
                           const HintJSON hint,
                           const TypeJSON type)
{
    ValueJSON *value = alloc_arena(arena,
                                   sizeof(ValueJSON));

    value->hint = hint;
    value->type = type;

    return value;
}
