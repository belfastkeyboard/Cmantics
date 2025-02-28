#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../internals/arena.h"
#include "../internals/parse.h"
#include "../internals/error.h"


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


int handle_integer(char *data,
                   size_t *offset)
{
    char *ptr;
    const size_t off = *offset;

    int result = (int)strtol(data + off,
                             &ptr,
                             10);

    *offset += ptr - (data + off);

    return result;
}


double handle_float(char *data,
                   size_t *offset)
{
    char *ptr;
    const size_t off = *offset;

    double result = strtod(data + off,
                           &ptr);

    *offset += ptr - (data + off);

    return result;
}


bool handle_boolean(char *data,
                    const char c,
                    size_t *offset)
{
    char boolean[6] = { 0 };
    bool result;

    size_t len = (c == 'f') ? 5 :
                              4;

    strncpy(boolean,
            data + *offset,
            len);

    *offset += len;

    if (strcmp(boolean,
               "true") == 0)
    {
        result = true;
    }
    else if (strcmp(boolean,
                    "false") == 0)
    {
        result = false;
    }
    else
    {
        throw(__FILE__,
              __FUNCTION__,
              __LINE__,
              "Error boolean type: %s",
              boolean);
    }

    return result;
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
