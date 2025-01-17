#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "../internals/parse.h"


void handle_string(Arena *arena,
                   char **src,
                   char *data,
                   size_t *offset)
{
    size_t len = strcspn(data + *offset,
                         "\"") + 1;

    *src = alloc_arena(arena,
                       len);

    strncpy(*src,
            data + *offset,
            len - 1);

    *offset += len;
}


int handle_integer(char *data,
                   size_t *offset)
{
    char *ptr;

    int result = (int)strtol(data + *offset,
                             &ptr,
                             10);

    *offset += ptr - (data + *offset);

    return result;
}


double handle_float(char *data,
                   size_t *offset)
{
    char *ptr;

    double result = strtod(data + *offset,
                           &ptr);

    *offset += ptr - (data + *offset);

    return result;
}


bool handle_boolean(char *data,
                    const char c,
                    size_t *offset)
{
    char boolean[6] = { 0 };

    size_t len = (c == 'f') ? 5 :
                              4;

    strncpy(boolean,
            data + *offset,
            len);

    *offset += len;

    return (strcmp(boolean,
                   "true") == 0);
}


void *handle_null(char *data,
                  size_t *offset)
{
    char null[5] = { 0 };

    size_t len = 4;

    strncpy(null,
            data + *offset,
            len);

    assert(strcmp(null,
                  "null") == 0);

    *offset += len;

    return NULL;
}
