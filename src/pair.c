#include <string.h>
#include "../internals/arena.h"
#include "../internals/array.h"
#include "../internals/obj.h"
#include "../internals/parse.h"
#include "../internals/value.h"


extern Arena *allocator;


Pair *make_pair(char *data,
                size_t *offset)
{
    Pair *pair = alloc_arena(allocator,
                             sizeof(Pair));

    handle_string(&pair->key,
                  data,
                  offset);

    *offset += strspn(data + *offset,
                      ": ");

    pair->value = make_value(data,
                             offset);

    return pair;
}
