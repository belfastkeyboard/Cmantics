#include "../../internals/csv/write.h"


static void write_int(FILE *file,
                      const long number)
{
    fprintf(file,
            "%ld",
            number);
}

static void write_float(FILE *file,
                        const double number)
{
    if (number == (int)number)
    {
        fprintf(file,
                "%.1f",
                number);
    }
    else
    {
        fprintf(file,
                "%.5g",
                number);
    }
}

static void write_bool(FILE *file,
                       Boolean boolean)
{
    fprintf(file,
            "%s",
            boolean.string);
}

static void write_string(FILE *file,
                         const char *string)
{
    fprintf(file,
            "%s",
            string);
}

static void write_value(FILE *file,
                        const ValueCSV *value)
{
    if (value->hint == CSV_INT)
    {
        write_int(file,
                  value->type.i);
    }
    else if (value->hint == CSV_FLOAT)
    {
        write_float(file,
                    value->type.f);
    }
    else if (value->hint == CSV_BOOL)
    {
        write_bool(file,
                   value->type.b);
    }
    else if (value->hint == CSV_STRING)
    {
        write_string(file,
                     value->type.s);
    }
}


void write_values(FILE *file,
                  const struct Table *table)
{
    for (int r = 0; r < table->rows; r++)
    {
        for (int c = 0; c < table->columns; c++)
        {
            const char *comma = (c < table->columns - 1) ? "," :
                                "\n";

            const ValueCSV *value = table->table[c][r];

            write_value(file,
                        value);

            fprintf(file,
                    "%s",
                    comma);
        }
    }
}