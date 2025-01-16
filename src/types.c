#include <ctype.h>
#include <string.h>
#include "../internals/types.h"


Hint determine_type(const char c)
{
    Hint value;

    if (c == '"')
    {
        value = HINT_STRING;
    }
    else if (isdigit(c) ||
             c == '-')
    {
        value = HINT_DIGIT;
    }
    else if (c == 'f' || c == 't')
    {
        value = HINT_BOOL;
    }
    else if (c == '[')
    {
        value = HINT_ARRAY;
    }
    else if (c == '{')
    {
        value = HINT_OBJECT;
    }
    else if (c == 'n')
    {
        value = HINT_NULL;
    }
    else
    {
        value = HINT_ERROR;
    }

    return value;
}


Hint determine_digit(char *data,
                     const size_t offset)
{
    Hint type = HINT_INT;

    size_t len = strspn(data + offset,
                        "-0123456789.");

    for (int i = 0; i < len; i++)
    {
        if (data[i] == '.')
        {
            type = HINT_FLOAT;

            break;
        }
    }

    return type;
}
