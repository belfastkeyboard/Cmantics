#include <string.h>
#include "../internals/arena.h"


char *make_string(const char *src,
                  Arena *arena)
{
    const size_t len = strlen(src);

    char *string = calloc_arena(arena,
                                len + 1);

    strncpy(string,
            src,
            len);

    return string;
}
