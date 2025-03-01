#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../../internals/arena.h"
#include "../../csv.h"
#include "../../internals/csv/table.h"
#include "../../internals/csv/parse.h"
#include "../../internals/csv/write.h"


typedef struct CSV
{
    Arena *arena;
    struct Table table;
} CSV;


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


void write_csv(const CSV *csv,
               const char *filepath)
{
    if (csv)
    {
        FILE *file = fopen(filepath,
                           "w");

        if (file)
        {
            write_values(file,
                         &csv->table);

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


void parse_csv(CSV *csv,
               const char *filepath)
{
    if (csv)
    {
        FILE *file = fopen(filepath,
                           "r");

        if (file)
        {
            parse_rows(file,
                       &csv->table,
                       csv->arena);

            fclose(file);
        }
    }
}


size_t columns_csv(const CSV *csv)
{
    size_t result = SIZE_MAX;

    if (csv)
    {
        result = csv->table.columns;
    }

    return result;
}

size_t rows_csv(const CSV *csv)
{
    size_t result = SIZE_MAX;

    if (csv)
    {
        result = csv->table.rows;
    }

    return result;
}
