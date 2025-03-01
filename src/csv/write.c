#include <stdio.h>
#include "../../csv.h"
#include "write.h"


void write_value(FILE *file,
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
