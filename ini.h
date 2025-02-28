#pragma once

#include <stdbool.h>
#include "internals/boolean.h"


typedef struct Array Array;
typedef struct INI INI;


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
