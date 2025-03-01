/**
 * INI Parser v1.1
 *
 * Provides utilities for reading, editing and writing INI.
 *
 *
 * INI has no enforced standard, and numerous dialects of INI exist.
 * This parser adheres to its own particular simple dialect.
 * It does not support nesting but does support arrays.
 *
 * This parser contains a unique Boolean value, that allows 'true'/'false',
 * 'yes'/'no', and 'on'/'off' to be parsed as valid boolean values.
 *
 * It is case-insensitive.
 *
 * Riain Ó Tuathail
 * 01-03-2025
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


// Allocate an INI parser that must be freed with destroy_ini()
INI *create_ini(void);

// Destroy an allocated INI parser, gracefully handles NULL INI pointers
void destroy_ini(INI **ini);


// Parse an INI file
void parse_ini(INI *ini,
               const char *filepath);

// Output contents of INI parser to file
void write_ini(const INI *ini,
               const char *filepath);


// Retrieve an editable value from the opaque INI parser, returns NULL on failure
ValueINI *get_ini(const INI *ini,
                  const char *section,
                  const char *key);


// Allocate a value with the JSON parser's allocator, returns default value
ValueINI *make_ini(INI* ini,
                   HintINI hint);


// Set a value in the INI Parser, if value does not exist it will be created
void set_ini(INI* ini,
             const char *section,
             const char *key,
             ValueINI *value);


// Erase a key-value pair from the INI Parser
void erase_ini(INI* ini,
               const char *section,
               const char *key);


// Get the number of values in an INI Array, returns SIZE_MAX on failure
size_t count_ini(ValueINI* array);
