#pragma once

#include <stdbool.h>


typedef struct INI INI;


typedef enum Hint
{
    INI_ERROR,
    INI_NULL,
    INI_STRING,
    INI_INT,
    INI_FLOAT,
    INI_BOOL,
    INI_ARRAY
} Hint;


typedef union Type
{
    void   *n;
    char   *s;
    long    i;
    double  f;
    bool    b;
} Type;


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
