#include <memory.h>
#include <stdio.h>
#include "../../internals/json/obj.h"
#include "../../internals/json/write.h"


#define INDENTATION_AMOUNT 4


static void write_indentation(FILE *file,
                              const int indentation)
{
    char buffer[indentation];

    memset(buffer,
           ' ',
           indentation);

    fwrite(buffer,
           sizeof(char),
           indentation,
           file);
}

static void write_new_line(FILE *file,
                           const int indentation)
{
    fputc('\n',
          file);

    write_indentation(file,
                      indentation);
}


static void write_key(FILE *file,
                      const char *string)
{
    fprintf(file,
            "\"%s\": ",
            string);
}


static void write_null(FILE *file)
{
    fprintf(file,
            "null");
}

static void write_string(FILE *file,
                         const char *string)
{
    fprintf(file,
            "\"%s\"",
            string);
}

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

static void write_boolean(FILE *file,
                          const bool boolean)
{
    const char *string = (boolean) ? "true" :
                                     "false";

    fprintf(file,
            "%s",
            string);
}


static void write_array(FILE *file,
                        const Array *array,
                        const int indentation,
                        Arena *arena)
{
    fputc('[',
          file);

    for (size_t i = 0; i < array->nmemb; ++i)
    {
        ValueJSON *value = array->values[i];

        if (value->hint == JSON_OBJECT)
        {
            write_new_line(file,
                           indentation + INDENTATION_AMOUNT);
        }

        write_value(file,
                    value,
                    indentation + INDENTATION_AMOUNT,
                    arena);

        if (i < array->nmemb - 1)
        {
            fprintf(file,
                    ", ");
        }
    }

    if (array->nmemb &&
        ((ValueJSON*)(array->values[array->nmemb - 1]))->hint == JSON_OBJECT)
    {
        write_new_line(file,
                       indentation);
    }

    fputc(']',
          file);
}


static void write_pair(FILE *file,
                       const struct DictPair pair,
                       const int indentation,
                       Arena *arena)
{
    write_key(file,
              pair.key);

    write_value(file,
                pair.value,
                indentation,
                arena);
}


static void write_object(FILE *file,
                         const Object *object,
                         const int indentation,
                         Arena *arena)
{
    fputc('{',
          file);

    write_new_line(file,
                   indentation + INDENTATION_AMOUNT);

    struct DictPair *pairs = get_iterable_pairs(object->dict,
                                                arena);
    for (int i = 0; i < object->dict->nmemb; ++i)
    {
        struct DictPair pair = pairs[i];

        write_pair(file,
                   pair,
                   indentation + INDENTATION_AMOUNT,
                   arena);

        if (i < object->dict->nmemb - 1)
        {
            fprintf(file,
                    ",");

            write_new_line(file,
                           indentation + INDENTATION_AMOUNT);
        }
    }

    write_new_line(file,
                   indentation);

    fputc('}',
          file);
}


void write_value(FILE *file,
                 const ValueJSON *value,
                 const int indentation,
                 Arena *arena)
{
    switch (value->hint)
    {
        case JSON_NULL:
            write_null(file);
            break;
        case JSON_OBJECT:
            write_object(file,
                         value->type.o,
                         indentation,
                         arena);
            break;
        case JSON_ARRAY:
            write_array(file,
                        value->type.a,
                        indentation,
                        arena);
            break;
        case JSON_STRING:
            write_string(file,
                         value->type.s);
            break;
        case JSON_INT:
            write_int(file,
                      value->type.i);
            break;
        case JSON_FLOAT:
            write_float(file,
                        value->type.f);
            break;
        case JSON_BOOL:
            write_boolean(file,
                          value->type.b);
            break;
        default:
            break;
    }
}
