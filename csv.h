/**
 * CSV Parser v1.0
 *
 * Provides utilities for reading, editing and writing CSV.
 *
 *
 * This parser cannot extend or retract rows or columns yet.
 * Values can only be modified in place.
 *
 * Riain Ó Tuathail
 * 01-03-2025
 *
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>


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
    bool     b;
} TypeCSV;


typedef struct ValueCSV
{
    HintCSV hint;
    TypeCSV type;
} ValueCSV;


// Allocate a CSV parser that must be freed with destroy_csv()
CSV *create_csv(void);

// Destroy an allocated CSV parser, gracefully handles NULL CSV pointers
void destroy_csv(CSV **csv);


// Parse a CSV file
void parse_csv(CSV *csv,
               const char *filepath);

// Output contents of CSV parser to file
void write_csv(const CSV *csv,
               const char *filepath);


// Retrieve an editable value from the opaque CSV parser, returns NULL on failure
// NOTE: modification to this value struct will be outputted to file on write_csv() call
ValueCSV *get_csv(const CSV *csv,
                  size_t column,
                  size_t row);
