#include <string.h>
#include "../internals/str.h"


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


char *handle_string(char *data,
                    size_t *offset,
                    Arena *arena)
{
    const size_t len = strcspn(data + *offset,
                               "\"");

    char *string = calloc_arena(arena,
                                len + 1);

    strncpy(string,
            data + *offset,
            len);

    *offset += len + 1;

    return string;
}

