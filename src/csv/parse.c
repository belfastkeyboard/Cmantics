#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../internals/csv/parse.h"
#include "../../internals/error.h"


#define STRING_NUMBERS "0123456789"


static void cull_newline(char *line)
{
    line[strlen(line) - 1] = '\0';
}

static bool is_sign(const char c)
{
    return c == '-' ||
           c == '+';
}

static bool has_scientific_notation(const char *token)
{
    bool result = false;

    if (token[0] == 'e' ||
        token[0] == 'E')
    {
        token++;

        if (is_sign(token[0]))
        {
            token++;
        }

        size_t i = strspn(token,
                          STRING_NUMBERS);

        if (!token[i])
        {
            result = true;
        }
    }

    return result;
}

static HintCSV determine_type(const char *token)
{
    HintCSV hint = CSV_STRING;

    const size_t length = strlen(token);

    char c = token[0];

    if (is_sign(c) &&
        length > 1)
    {
        c = *(++token);
    }

    if (isdigit(c))
    {
        size_t i = strspn(token,
                          STRING_NUMBERS);

        c = token[i];

        if (c == '\0')
        {
            hint = CSV_INT;
        }
        else if (c == '.')
        {
            const char *f = token + i + 1;

            i = strspn(f,
                       STRING_NUMBERS);

            c = f[i];

            if (c == '\0' ||
                has_scientific_notation(f + i))
            {
                hint = CSV_FLOAT;
            }
        }
    }

    if (hint == CSV_STRING)
    {
        if (length == 0)
        {
            hint = CSV_NULL;
        }
        else if (strcasecmp(token,
                            "true") == 0 ||
                 strcasecmp(token,
                            "false") == 0)
        {
            hint = CSV_BOOL;
        }
    }

    return hint;
}


static char *handle_string(const char *token,
                           Arena *arena)
{
    const size_t len = strlen(token);

    char *string = calloc_arena(arena,
                                len + 1);

    strncpy(string,
            token,
            len);

    return string;
}

static long handle_integer(const char *token)
{
    long result = strtol(token,
                         NULL,
                         10);

    return result;
}

static double handle_float(const char *token)
{
    double result = strtod(token,
                           NULL);

    return result;
}

static Boolean handle_boolean(const char *token,
                              Arena *arena)
{
    bool result;

    if (strcasecmp(token,
                   "true") == 0)
    {
        result = true;
    }
    else if (strcasecmp(token,
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
              token);
    }

    return make_boolean(result,
                        token,
                        arena);
}


size_t count_columns(const char *header)
{
    size_t result = 1;

    char c;
    while ((c = *header))
    {
        if (c == ',')
        {
            result++;
        }

        header++;
    }

    return result;
}


void resize_rows(struct Table *table)
{
    if (table->rows >= table->r_capacity)
    {
        const size_t new_cap = (table->r_capacity) ? table->r_capacity * (2 + table->columns) :
                               1;

        for (int i = 0; i < table->columns; i++)
        {
            ValueCSV **row = table->table[i];

            table->table[i] = realloc(row,
                                      sizeof(ValueCSV*) * new_cap);
        }

        table->r_capacity = new_cap;
    }
}

static void reserve_table(struct Table *table,
                          const size_t columns,
                          const size_t rows)
{
    table->columns = columns;
    table->c_capacity = columns;
    table->r_capacity = rows;

    ValueCSV ***temp = realloc(table->table,
                               sizeof(ValueCSV**) * table->c_capacity);

    if (temp)
    {
        table->table = temp;
    }

    for (int i = 0; i < table->columns; i++)
    {
        ValueCSV **row = table->table[i];

        table->table[i] = realloc(row,
                                  sizeof(ValueCSV*) * table->r_capacity);
    }
}


static ValueCSV *parse_value(char *token,
                             Arena *arena)
{
    TypeCSV type;
    HintCSV hint = determine_type(token);

    if (hint == CSV_STRING)
    {
        type.s = handle_string(token,
                               arena);
    }
    else if (hint == CSV_INT)
    {
        type.i = handle_integer(token);
    }
    else if (hint == CSV_FLOAT)
    {
        type.f = handle_float(token);
    }
    else if (hint == CSV_BOOL)
    {
        type.b = handle_boolean(token,
                                arena);
    }
    else
    {
        type.n = NULL;
    }

    ValueCSV *value = make_value_csv(arena,
                                     hint,
                                     type);
    return value;
}


static void parse_row(struct Table *table,
                      char *line,
                      Arena *arena)
{
    char *save;
    char *token = strtok_r(line,
                           ", ",
                           &save);

    size_t column = 0;
    while (token)
    {
        ValueCSV *value = parse_value(token,
                                      arena);

        table->table[column++][table->rows] = value;

        token = strtok_r(NULL,
                         ", ",
                         &save);
    }

    table->rows++;
}


void parse_rows(FILE *file,
                struct Table *table,
                Arena *arena)
{
    char *line = NULL;
    size_t n;

    if (getline(&line,
                &n,
                file) != -1)
    {
        reserve_table(table,
                      count_columns(line),
                      1);

        cull_newline(line);

        parse_row(table,
                  line,
                  arena);

        while (getline(&line,
                       &n,
                       file) != -1)
        {
            resize_rows(table);

            cull_newline(line);

            parse_row(table,
                      line,
                      arena);
        }
    }
}
