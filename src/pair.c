#include <string.h>
#include "../internals/arena.h"
#include "../internals/array.h"
#include "../internals/obj.h"
#include "../internals/str.h"
#include "../internals/value.h"


struct DictPair parse_pair(char *data,
                           size_t *offset,
                           Array *meta,
                           Arena *arena)
{
    const char *key = handle_string(data,
                                    offset,
                                    arena);

    *offset += strspn(data + *offset,
                      ": ");

    void *value = parse_value(data,
                              offset,
                              meta,
                              arena);

    struct DictPair pair = {
        .key = key,
        .value = value
    };

    return pair;
}
