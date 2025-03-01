#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../internals/arena.h"
#include "../../internals/array.h"
#include "../../internals/dict.h"
#include "../../internals/str.h"
#include "../../internals/ini/value.h"
#include "../../internals/ini/section.h"
#include "../../internals/ini/parse.h"
#include "../../internals/error.h"


#define STRING_NUMBERS "0123456789"


static bool is_sign(const char c)
{
    return c == '-' ||
           c == '+';
}

static bool has_scientific_notation(const char *line)
{
    bool result = false;

    if (line[0] == 'e' ||
        line[0] == 'E')
    {
        line++;

        if (is_sign(line[0]))
        {
            line++;
        }

        size_t i = strspn(line,
                          STRING_NUMBERS);

        if (!line[i])
        {
            result = true;
        }
    }

    return result;
}


static HintINI determine_type(const char *line)
{
    HintINI hint = INI_STRING;

    const size_t length = strlen(line);

    char c = line[0];

    if (is_sign(c) &&
        length > 1)
    {
        c = *(++line);
    }

    if (isdigit(c))
    {
        size_t i = strspn(line,
                          STRING_NUMBERS);

        c = line[i];

        if (c == '\0')
        {
            hint = INI_INT;
        }
        else if (c == '.')
        {
            const char *f = line + i + 1;

            i = strspn(f,
                       STRING_NUMBERS);

            c = f[i];

            if (c == '\0' ||
                has_scientific_notation(f + i))
            {
                hint = INI_FLOAT;
            }
        }
    }

    if (hint == INI_STRING)
    {
        if (length == 0)
        {
            hint = INI_NULL;
        }
        else if (strcasecmp(line,
                            "no") == 0 ||
                 strcasecmp(line,
                            "on") == 0 ||
                 strcasecmp(line,
                            "yes") == 0 ||
                 strcasecmp(line,
                            "off") == 0 ||
                 strcasecmp(line,
                            "true") == 0 ||
                 strcasecmp(line,
                            "false") == 0)
        {
            hint = INI_BOOL;
        }
        else if (strcspn(line,
                         ",") != length)
        {
            hint = INI_ARRAY;
        }
    }

    return hint;
}


static char *handle_string(const char *line,
                           Arena *arena)
{
    const size_t len = strlen(line);

    char *string = calloc_arena(arena,
                                len + 1);

    strncpy(string,
            line,
            len);

    return string;
}

static long handle_integer(const char *line)
{
    long result = strtol(line,
                         NULL,
                         10);

    return result;
}

static double handle_float(const char *line)
{
    double result = strtod(line,
                           NULL);

    return result;
}

static Boolean handle_boolean(const char *line,
                              Arena *arena)
{
    char boolean[6] = { 0 };
    bool result;

    size_t len = strlen(line);

    strncpy(boolean,
            line,
            len);

    if (strcmp(boolean,
               "on") == 0 ||
        strcmp(boolean,
               "yes") == 0 ||
        strcmp(boolean,
               "true") == 0)
    {
        result = true;
    }
    else if (strcmp(boolean,
                    "no") == 0 ||
             strcmp(boolean,
                    "off") == 0 ||
             strcmp(boolean,
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

    return make_boolean(result,
                        line,
                        arena);
}


Array *parse_array(char *line,
                   Array *meta,
                   Arena *arena)
{
    Array *array = make_array(meta,
                              arena);


    char *c_ptr;
    char *token = strtok_r(line,
                           ", ",
                           &c_ptr);

    while (token)
    {
        ValueINI *value = parse_value_ini(token,
                                          meta,
                                          arena);

        push_array(array,
                   value);

        token = strtok_r(NULL,
                         ", ",
                         &c_ptr);
    }

    return array;
}


ValueINI *parse_value_ini(char *line,
                          Array *meta,
                          Arena *arena)
{
    TypeINI type;
    HintINI hint = determine_type(line);

    if (hint == INI_STRING)
    {
        type.s = handle_string(line,
                               arena);
    }
    else if (hint == INI_INT)
    {
        type.i = handle_integer(line);
    }
    else if (hint == INI_FLOAT)
    {
        type.f = handle_float(line);
    }
    else if (hint == INI_BOOL)
    {
        type.b = handle_boolean(line,
                                arena);
    }
    else if (hint == INI_ARRAY)
    {
        type.a = parse_array(line,
                             meta,
                             arena);
    }
    else
    {
        type.n = NULL;
    }

    ValueINI *value = make_value_ini(arena,
                                     hint,
                                     type);
    return value;
}


static char *section_substr(char *line)
{
    const size_t i = strcspn(line + 1,
                             "]");

    line[i + 1] = '\0';

    return line + 1;
}


static char *parse_string(char *line,
                          Arena *arena)
{
    const size_t i = strcspn(line,
                             "\n");

    line[i] = '\0';

    char *string = make_string(line,
                               arena);

    return string;
}


static void parse_pair(char *line,
                       struct Dict *dict,
                       Array *meta,
                       Arena *arena)
{
    const size_t i = strcspn(line,
                             "=");

    line[i] = '\0';

    const char *key = parse_string(line,
                                   arena);

    char *v_string = parse_string(line + i + 1,
                                  arena);

    ValueINI *value = parse_value_ini(v_string,
                                      meta,
                                      arena);

    insert_dict(dict,
                key,
                (void*)value);
}


static void parse_pairs(struct Section *header,
                        FILE *file,
                        Array *meta,
                        Arena *arena)
{
    char *line = NULL;
    size_t len = 0;

    while (getline(&line,
                   &len,
                   file) != -1)
    {
        if (line[0] == '\n')
        {
            break;
        }

        parse_pair(line,
                   header->pairs,
                   meta,
                   arena);
    }

    free(line);
}


struct Section *parse_section(FILE *file,
                              char *line,
                              Array *meta,
                              Arena *arena)
{
    char *substr = section_substr(line);

    const char *name = parse_string(substr,
                                    arena);

    struct Section *header = create_header(name,
                                           arena);

    parse_pairs(header,
                file,
                meta,
                arena);

    return header;
}
