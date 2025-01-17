#include <memory.h>
#include <stdio.h>
#include "../internals/array.h"
#include "../internals/write.h"


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
    fprintf(file,
            "%.2f",
            number);
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


static void write_value(FILE *file,
                        const Value *value,
                        const int indentation)
{
    switch (value->hint)
    {
        case HINT_NULL:
            write_null(file);
            break;
        case HINT_OBJECT:
            write_object(file,
                         value->type.o,
                         indentation);
            break;
        case HINT_ARRAY:
            write_array(file,
                        value->type.a,
                        indentation);
            break;
        case HINT_STRING:
            write_string(file,
                         value->type.s);
            break;
        case HINT_INT:
            write_int(file,
                      value->type.i);
            break;
        case HINT_FLOAT:
            write_float(file,
                        value->type.f);
            break;
        case HINT_BOOL:
            write_boolean(file,
                          value->type.b);
            break;
        default:
            break;
    }
}


void write_array(FILE *file,
                 const Array *array,
                 const int indentation)
{
    fputc('[',
          file);

    for (size_t i = 0; i < array->size; ++i)
    {
        Value *value = array->values[i];

        if (value->hint == HINT_OBJECT)
        {
            write_new_line(file,
                           indentation + INDENTATION_AMOUNT);
        }

        write_value(file,
                    value,
                    indentation + INDENTATION_AMOUNT);

        if (i < array->size - 1)
        {
            fprintf(file,
                    ", ");
        }
    }

    if (array->values[array->size - 1]->hint == HINT_OBJECT)
    {
        write_new_line(file,
                       indentation);
    }

    fputc(']',
          file);
}


static void write_pair(FILE *file,
                       const Pair pair,
                       const int indentation)
{
    write_key(file,
              pair.key);

    write_value(file,
                &pair.value,
                indentation);
}


void write_object(FILE *file,
                  const Object *object,
                  const int indentation)
{
    fputc('{',
          file);

    write_new_line(file,
                   indentation + INDENTATION_AMOUNT);

    for (size_t i = 0; i < object->pair_size; i++)
    {
        const Pair pair = object->kv_pairs[i];

        write_pair(file,
                   pair,
                   indentation + INDENTATION_AMOUNT);

        if (i < object->pair_size - 1)
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
