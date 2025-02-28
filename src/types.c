#include <ctype.h>
#include <string.h>
#include "../internals/types.h"


Hint determine_type(const char c,
                    char *data,
                    const size_t offset)
{
    Hint hint;

    if (c == '"')
    {
        hint = JSON_STRING;
    }
    else if (isdigit(c) ||
             c == '-')
    {
        hint = JSON_INT;

        size_t len = strspn(data + offset,
                            "+-0123456789.eE");

        for (int i = 0; i < len; i++)
        {
            if (data[offset + i] == '.' ||
                data[offset + i] == 'e' ||
                data[offset + i] == 'E')
            {
                hint = JSON_FLOAT;

                break;
            }
        }
    }
    else if (c == 'f' || c == 't')
    {
        hint = JSON_BOOL;
    }
    else if (c == '[')
    {
        hint = JSON_ARRAY;
    }
    else if (c == '{')
    {
        hint = JSON_OBJECT;
    }
    else if (c == 'n')
    {
        hint = JSON_NULL;
    }
    else
    {
        hint = JSON_ERROR;
    }

    return hint;
}
