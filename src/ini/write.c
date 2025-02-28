#include "../../internals/ini/section.h"
#include "../../internals/ini/value.h"
#include "../../internals/ini/write.h"
#include "../../json.h"
#include "../../internals/array.h"
#include "../../internals/error.h"


static void write_value(FILE *file,
                        const ValueINI *value);


static void write_string(FILE *file,
                         const char *string)
{
    fprintf(file,
            "%s",
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

static void write_array(FILE *file,
                        const Array *array)
{
    for (int i = 0; i < array->nmemb; ++i)
    {
        ValueINI *value = array->values[i];

        write_value(file,
                    value);

        if (i < array->nmemb - 1)
        {
            fprintf(file,
                    ", ");
        }
    }
}

static void write_boolean(FILE *file,
                          const char *boolean)
{
    fprintf(file,
            "%s",
            boolean);
}

void write_value(FILE *file,
                 const ValueINI *value)
{
    HintINI hint = value->hint;

    switch (hint)
    {
        case INI_STRING:
            write_string(file,
                         value->type.s);
            break;
        case INI_INT:
            write_int(file,
                      value->type.i);
            break;
        case INI_FLOAT:
            write_float(file,
                        value->type.f);
            break;
        case INI_ARRAY:
            write_array(file,
                        value->type.a);
            break;
        case INI_BOOL:
            write_boolean(file,
                          value->type.b.string);
            break;
        case INI_NULL:
            break;
        default:
            throw(__FILE__,
                  __FUNCTION__,
                  __LINE__,
                  "Error hint type: %d",
                  hint);
    }
}

void write_section(FILE *file,
                   const struct DictPair section,
                   Arena *arena)
{
    const char *name = section.key;

    fprintf(file,
            "[%s]\n",
            name);

    struct Section *sec = section.value;
    const size_t nmemb = sec->pairs->nmemb;
    struct DictPair *pairs = get_iterable_pairs(sec->pairs,
                                                arena);

    for (int j = 0; j < nmemb; ++j)
    {
        const char *k = pairs[j].key;

        fprintf(file,
                "%s=",
                k);

        const ValueINI *value = pairs[j].value;

        write_value(file,
                    value);

        fprintf(file,
                "\n");
    }
}