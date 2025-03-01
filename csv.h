#pragma once

#include <stdbool.h>
#include "internals/boolean.h"


typedef struct CSV CSV;


typedef enum HintCSV
{
    CSV_ERROR,
    CSV_NULL,
    CSV_STRING,
    CSV_INT,
    CSV_FLOAT,
    CSV_BOOL,
} HintCSV;


typedef union TypeCSV
{
    void    *n;
    char    *s;
    long     i;
    double   f;
    Boolean  b;
} TypeCSV;


typedef struct ValueCSV
{
    HintCSV hint;
    TypeCSV type;
} ValueCSV;


CSV *create_csv(void);

void destroy_csv(CSV **csv);


void parse_csv(CSV *csv,
               const char *filepath);


void write_csv(const CSV *csv,
               const char *filepath);


ValueCSV *get_csv(const CSV *csv,
                  size_t column,
                  size_t row);
