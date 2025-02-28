#include "../../internals/json/obj.h"
#include "../../internals/json/value.h"


Object *make_object(Array *meta,
                    Arena *arena)
{
    Object *object = alloc_arena(arena,
                                 sizeof(Object));

    object->dict = create_dict(arena,
                               true);

    HintJSON hint = JSON_OBJECT;
    TypeJSON type = {
        .o = object
    };

    ValueJSON *value = make_value_json(arena,
                                       hint,
                                       type);

    push_array(meta,
               value);

    return object;
}
