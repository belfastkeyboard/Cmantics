#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "../internals/array.h"
#include "../internals/write.h"
#include "../internals/value.h"
#include "../internals/error.h"


typedef struct JSON
{
    Arena *arena;
    Array *meta;
    Value *value;
} JSON;


static size_t get_file_size(FILE *file)
{
    assert(file);

    ssize_t fsize;

    rewind(file);
    fseek(file, 0, SEEK_END);

    fsize = ftell(file);

    rewind(file);

    return fsize;
}


JSON *create_json(void)
{
    Arena *arena = create_arena();

    JSON *json = alloc_arena(arena,
                             sizeof(JSON));

    Hint hint = JSON_NULL;
    Type type = {
        .n = NULL
    };

    json->arena = arena;
    json->meta = create_array(json->arena);
    json->value = make_value(json->arena,
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
            Value *value = (*json)->meta->values[i];

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


Value *get_json(JSON *json)
{
    return json->value;
}

Value *lookup_json(Value *object,
                   const char *key)
{
    Value *result = NULL;

    if (object->hint == JSON_OBJECT)
    {
        result = find_dict(object->type.o->dict,
                           key);
    }

    return result;
}

Value *scan_json(Value *array,
                 size_t index)
{
    Value *result = NULL;

    if (array->hint == JSON_ARRAY &&
        index < array->type.a->nmemb)
    {
        result = array->type.a->values[index];
    }

    return result;
}


Value *make_json(JSON* json,
                 Hint hint)
{
    Value *value = alloc_arena(json->arena,
                               sizeof(Value));

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


void push_json(Value *array,
               Value *value)
{
    if (array->hint == JSON_ARRAY)
    {
        push_array(array->type.a,
                   value);
    }
}

void pop_json(Value *array,
               const size_t index)
{
    if (array->hint == JSON_ARRAY)
    {
        pop_array(array->type.a,
                  index);
    }
}


void insert_json(Value *object,
                 const char *key,
                 Value *value)
{
    if (object->hint == JSON_OBJECT)
    {
        insert_dict(object->type.o->dict,
                    key,
                    value);
    }
}

void erase_json(Value *object,
                const char *key)
{
    if (object->hint == JSON_OBJECT)
    {
        erase_dict(object->type.o->dict,
                   key);
    }
}


size_t count_json(Value* container)
{
    size_t result = -1;

    if (container->hint == JSON_ARRAY)
    {
        result = container->type.a->nmemb;
    }
    else if (container->hint == JSON_OBJECT)
    {
        result = container->type.o->dict->nmemb;
    }

    return result;
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

        Value *value = parse_value(fdata,
                                   &offset,
                                   json->meta,
                                   json->arena);

        json->value = value;

        free(fdata);
    }

    fclose(file);
}

void write_json(const JSON *json,
                const char *path)
{
    FILE *file = fopen(path,
                       "w");

    assert(file);

    write_value(file,
                json->value,
                0,
                json->arena);

    fclose(file);
}
