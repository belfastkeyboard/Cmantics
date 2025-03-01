#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "../../internals/json/obj.h"
#include "../../internals/json/write.h"
#include "../../internals/json/parse.h"
#include "../../internals/error.h"


typedef struct JSON
{
    Arena *arena;
    Array *meta;
    ValueJSON *value;
} JSON;


static size_t get_file_size(FILE *file)
{
    assert(file);

    ssize_t fsize;

    rewind(file);

    fseek(file,
          0,
          SEEK_END);

    fsize = ftell(file);

    rewind(file);

    return fsize;
}


JSON *create_json(void)
{
    Arena *arena = create_arena();

    JSON *json = alloc_arena(arena,
                             sizeof(JSON));

    HintJSON hint = JSON_NULL;
    TypeJSON type = {
        .n = NULL
    };

    json->arena = arena;
    json->meta = create_array(json->arena);
    json->value = make_value_json(json->arena,
                                  hint,
                                  type);

    return json;
}


void destroy_json(JSON **json)
{
    if (*json)
    {
        const size_t count = (*json)->meta->nmemb;

        for (int i = 0; i < count; ++i)
        {
            ValueJSON *value = (*json)->meta->values[i];

            if (value->hint == JSON_ARRAY)
            {
                destroy_array(value->type.a);
            }
            else if (value->hint == JSON_OBJECT)
            {
                destroy_dict(value->type.o->dict);
            }
            else
            {
                throw(__FILE__,
                      __FUNCTION__,
                      __LINE__,
                      "Error hint type: %d",
                      value->hint);
            }
        }

        destroy_array((*json)->meta);

        Arena *arena = (*json)->arena;
        destroy_arena(&arena);

        *json = NULL;
    }
}


void parse_json(JSON *json,
                const char *path)
{
    FILE *file = fopen(path,
                       "r");

    if (file)
    {
        size_t offset = 0;
        size_t fsize = get_file_size(file);

        char *fdata = calloc(sizeof(char),
                             fsize + 1);

        fread(fdata,
              sizeof(char),
              fsize,
              file);

        ValueJSON *value = parse_value_json(fdata,
                                            &offset,
                                            json->meta,
                                            json->arena);

        json->value = value;

        free(fdata);

        fclose(file);
    }
}

void write_json(const JSON *json,
                const char *path)
{
    if (json)
    {
        FILE *file = fopen(path,
                           "w");

        if (file)
        {
            write_value(file,
                        json->value,
                        0,
                        json->arena);
        }

        fclose(file);
    }
}


ValueJSON *get_json(JSON *json)
{
    return json->value;
}

ValueJSON *lookup_json(ValueJSON *object,
                       const char *key)
{
    ValueJSON *result = NULL;

    if (object->hint == JSON_OBJECT)
    {
        result = find_dict(object->type.o->dict,
                           key);
    }

    return result;
}

ValueJSON *scan_json(ValueJSON *array,
                     size_t index)
{
    ValueJSON *result = NULL;

    if (array->hint == JSON_ARRAY &&
        index < array->type.a->nmemb)
    {
        result = array->type.a->values[index];
    }

    return result;
}


ValueJSON *make_json(JSON* json,
                     HintJSON hint)
{
    ValueJSON *value = alloc_arena(json->arena,
                                   sizeof(ValueJSON));

    value->hint = hint;

    if (hint == JSON_INT)
    {
        value->type.i = 0;
    }
    else if (hint == JSON_FLOAT)
    {
        value->type.f = 0.0f;
    }
    else if (hint == JSON_BOOL)
    {
        value->type.b = false;
    }
    else if (hint == JSON_STRING)
    {
        value->type.s = "";
    }
    else if (hint == JSON_NULL)
    {
        value->type.n = NULL;
    }
    else if (hint == JSON_ARRAY)
    {
        value->type.a = make_array(json->meta,
                                   json->arena);
    }
    else if (hint == JSON_OBJECT)
    {
        value->type.o = make_object(json->meta,
                                    json->arena);
    }
    else
    {
        throw(__FILE__,
              __FUNCTION__,
              __LINE__,
              "Error hint type: %d",
              hint);
    }

    return value;
}


void push_json(ValueJSON *array,
               ValueJSON *value)
{
    if (array->hint == JSON_ARRAY)
    {
        push_array(array->type.a,
                   value);
    }
}

void pop_json(ValueJSON *array,
              const size_t index)
{
    if (array->hint == JSON_ARRAY)
    {
        pop_array(array->type.a,
                  index);
    }
}


void insert_json(ValueJSON *object,
                 const char *key,
                 ValueJSON *value)
{
    if (object->hint == JSON_OBJECT)
    {
        insert_dict(object->type.o->dict,
                    key,
                    value);
    }
}

void erase_json(ValueJSON *object,
                const char *key)
{
    if (object->hint == JSON_OBJECT)
    {
        erase_dict(object->type.o->dict,
                   key);
    }
}


size_t count_json(ValueJSON* container)
{
    size_t result = SIZE_MAX;

    if (container)
    {
        if (container->hint == JSON_ARRAY)
        {
            result = container->type.a->nmemb;
        }
        else if (container->hint == JSON_OBJECT)
        {
            result = container->type.o->dict->nmemb;
        }
    }

    return result;
}
