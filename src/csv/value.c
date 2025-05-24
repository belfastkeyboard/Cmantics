#include "../../internals/arena.h"
#include "../../internals/csv/value.h"


ValueCSV *make_value_csv(Arena *arena,
                         HintCSV hint,
                         TypeCSV type)
{
    ValueCSV *value = alloc_cmantics_arena(arena,
                                           sizeof(ValueCSV));

    value->hint = hint;
    value->type = type;

    return value;
}
