#include "../internals/str.h"
#include "../internals/ini/boolean.h"


Boolean make_boolean(bool value,
                     const char *string,
                     Arena *arena)
{
    char *copy = make_string(string,
                             arena);

    Boolean boolean = {
        .boolean = value,
        .string = copy
    };

    return boolean;
}
