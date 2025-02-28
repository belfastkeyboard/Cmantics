#include <string.h>
#include "../internals/arena.h"
#include "../internals/array.h"
#include "../internals/obj.h"
#include "../internals/parse.h"
#include "../internals/value.h"
#include "../internals/json.h"


struct DictPair parse_pair(JSON *json,
                           char *data,
                           size_t *offset)
{
    const char *key = handle_string(data,
                                    offset,
                                    json->arena);

    *offset += strspn(data + *offset,
                      ": ");

    void *value = parse_value(json,
                              data,
                              offset);

    struct DictPair pair = {
        .key = key,
        .value = value
    };

    return pair;
}
