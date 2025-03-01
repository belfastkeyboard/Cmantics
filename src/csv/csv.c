#include <ctype.h>
#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../internals/arena.h"
#include "../internals/str.h"
#include "../csv.h"
#include "../internals/error.h"


#define STRING_NUMBERS "0123456789"


struct Table
{
    size_t columns;
    size_t c_capacity;
    size_t rows;
    size_t r_capacity;
    ValueCSV ***table;
};

typedef struct CSV
{
    Arena *arena;
    struct Table table;
} CSV;

size_t shift_left(char *line,
                size_t i,
                const size_t len)
{
    memmove(line + i,
            line + i + 1,
            (len - i));

    return len - 1;
}

void cull_newline(char *line)
{
    line[strlen(line) - 1] = '\0';
}


CSV *create_csv(void)
{
    Arena *arena = create_arena();

    CSV *csv = calloc_arena(arena,
                            sizeof(CSV));

    csv->arena = arena;

    return csv;
}

void destroy_csv(CSV **csv)
{
    if (*csv)
    {
        for (int i = 0; i < (*csv)->table.columns; ++i)
        {
            free((*csv)->table.table[i]);
        }

        free((*csv)->table.table);

        Arena *arena = (*csv)->arena;
        destroy_arena(&arena);

        *csv = NULL;
    }
}


static void write_value(FILE *file,
                        ValueCSV *value)
{
    if (value->hint == CSV_STRING)
    {
        fprintf(file,
                "%s",
                value->type.s);
    }
    else if (value->hint == CSV_BOOL)
    {
        fprintf(file,
                "%s",
                value->type.b.string);
    }
    else if (value->hint == CSV_FLOAT)
    {
        if (value->type.f == (int)value->type.f)
        {
            fprintf(file,
                    "%.1f",
                    value->type.f);
        }
        else
        {
            fprintf(file,
                    "%.5g",
                    value->type.f);
        }
    }
    else if (value->hint == CSV_INT)
    {
        fprintf(file,
                "%ld",
                value->type.i);
    }
}

void write_csv(const CSV *csv,
               const char *filepath)
{
    if (csv)
    {
        FILE *file = fopen(filepath,
                           "w");

        if (file)
        {
            for (int r = 0; r < csv->table.rows; r++)
            {
                for (int c = 0; c < csv->table.columns; c++)
                {
                    const char *comma = (c < csv->table.columns - 1) ? "," :
                                                                       "\n";

                    ValueCSV *value = csv->table.table[c][r];

                    write_value(file,
                                value);

                    fprintf(file,
                            "%s",
                            comma);
                }
            }

            fclose(file);
        }
    }
}


ValueCSV *get_csv(const CSV *csv,
                  size_t column,
                  size_t row)
{
    ValueCSV *value = NULL;

    if (column < csv->table.columns &&
        row < csv->table.rows)
    {
        value = csv->table.table[column][row];
    }

    return value;
}

void set_csv(CSV* csv,
             size_t column,
             size_t row,
             ValueCSV *value)
{
    if (column < csv->table.columns &&
        row < csv->table.rows)
    {
        csv->table.table[column][row] = value;
    }
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

void reserve_table(struct Table *table,
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

ValueCSV *make_value_csv(Arena *arena,
                         HintCSV hint,
                         TypeCSV type)
{
    ValueCSV *value = alloc_arena(arena,
                                  sizeof(ValueCSV));

    value->hint = hint;
    value->type = type;

    return value;
}

void parse_header(struct Table *table,
                  char *line,
                  size_t columns,
                  Arena *arena)
{
    reserve_table(table,
                  columns,
                  1);

    cull_newline(line);

    char *save;
    char *token = strtok_r(line,
                           ", ",
                           &save);

    size_t column = 0;
    while (token)
    {
        TypeCSV type = {
            .s = make_string(token,
                             arena)
        };

        ValueCSV *value = make_value_csv(arena,
                                         CSV_STRING,
                                         type);

        table->table[column][table->rows] = value;

        token = strtok_r(NULL,
                         ", ",
                         &save);
    }

    table->rows++;
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

ValueCSV *parse_value(char *token,
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

void parse_row(struct Table *table,
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


void parse_csv(CSV *csv,
               const char *filepath)
{
    if (csv)
    {
        FILE *file = fopen(filepath,
                           "r");

        if (file)
        {
            char *line = NULL;
            size_t n;

            if (getline(&line,
                    &n,
                    file) != -1)
            {
                reserve_table(&csv->table,
                              count_columns(line),
                              1);

                cull_newline(line);

                parse_row(&csv->table,
                          line,
                          csv->arena);

                while (getline(&line,
                               &n,
                               file) != -1)
                {
                    cull_newline(line);

                    resize_rows(&csv->table);

                    parse_row(&csv->table,
                              line,
                              csv->arena);
                }
            }

            fclose(file);
        }
    }
}
