/**
 * INI Parser v1.0
 *
 * Provides utilities for reading, editing and writing INI.
 *
 *
 * INI has no enforced standard, and numerous dialects of INI exist.
 * This parser adheres to its own particular simple dialect.
 * It does not support nesting.
 *
 * Riain Ó Tuathail
 * 2025-03-01
 *
 */

#pragma once

#include <stdbool.h>


typedef struct Array Array;
typedef struct INI INI;


typedef struct Boolean
{
    bool boolean;
    char *string;
} Boolean;

typedef enum HintINI
{
    INI_ERROR,
    INI_NULL,
    INI_STRING,
    INI_INT,
    INI_FLOAT,
    INI_BOOL,
    INI_ARRAY
} HintINI;


typedef union TypeINI
{
    void    *n;
    char    *s;
    long     i;
    double   f;
    Boolean  b;
    Array   *a;
} TypeINI;


typedef struct ValueINI
{
    HintINI hint;
    TypeINI type;
} ValueINI;


INI *create_ini(void);

void destroy_ini(INI **ini);


void parse_ini(INI *ini,
               const char *filepath);

void write_ini(const INI *ini,
               const char *filepath);


const char *get_ini(const INI *ini,
                    const char *header,
                    const char *key);


void set_ini(INI* ini,
             const char *header,
             const char *key,
             const char *value);

void erase_ini(INI* ini,
               const char *header,
               const char *key);
