#include <stddef.h>
#include <string.h>
#include "../internals/ext.h"

#define EXT_LEN 5



bool is_json_file(const char *path)
{
    bool is_json = false;

    const size_t len = strlen(path);

    if (len > EXT_LEN)
    {
        if (strcmp(path + len - EXT_LEN,
                   ".json") == 0)
        {
            is_json = true;
        }
    }

    return is_json;
}
