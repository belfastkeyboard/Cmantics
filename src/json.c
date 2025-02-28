#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "../internals/symbols.h"
#include "../internals/array.h"
#include "../internals/eval.h"
#include "../internals/get.h"
#include "../internals/write.h"
#include "../internals/value.h"
#include "../internals/json.h"
#include "../internals/error.h"


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
    Arena *arena = create_arena(KB(4));

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
        value->type.a = make_array(json);
    }
    else if (hint == JSON_OBJECT)
    {
        value->type.o = make_object(json);
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

/*
JSON *json_find(JSON *value,
                const char *key)
{
    JSON *result = NULL;

    if (value->hint == JSON_OBJECT)
    {
        size_t i;
        Pair *pair = scan_object(value->type.o,
                                 key,
                                 &i);

        if (pair)
        {
            result = pair->value;
        }
    }

    return result;
}


JSON *json_make_value(const Hint hint,
                      void *type)
{

    JSON *json = NULL;

    if (hint != JSON_OBJECT &&
        hint != JSON_ARRAY)
    {
        json = alloc_arena(allocator,
                           sizeof(JSON));

        json->hint = hint;
    }

    switch (hint)
    {
        case JSON_INT:
            json->type.i = *(int*)type;
            break;
        case JSON_FLOAT:
            json->type.f = *(float*)type;
            break;
        case JSON_NULL:
            json->type.n = NULL;
            break;
        case JSON_BOOL:
            json->type.b = *(bool*)type;
            break;
        case JSON_STRING:
            json->type.s = (char*)type;
            break;
        case JSON_ARRAY:
            json = json_make_array();
            break;
        case JSON_OBJECT:
            json = json_make_object();
            break;
        default:
            break;
    }

    return json;
}


JSON *json_make_object(void)
{
    JSON *json = NULL;

    if (allocator)
    {
        json = alloc_arena(allocator,
                           sizeof(JSON));

        json->hint = JSON_OBJECT;

        Object *object = alloc_arena(allocator,
                                     sizeof(Object));

        object->pairs = NULL;
        object->size = 0;

        json->type.o = object;
    }

    return json;
}

void json_push_object(Object *object,
                      char *key,
                      JSON *value)
{
    size_t i;
    Pair *existing_kv_pair = scan_object(object,
                                         key,
                                         &i);

    if (!existing_kv_pair)
    {
        const size_t size = object->size;
        const size_t n_size = size + 1;

        Pair **pairs = alloc_arena(allocator,
                                  sizeof(Pair*) * n_size);

        memcpy(pairs,
               object->pairs,
               sizeof(Pair*) * size);

        Pair *pair = alloc_arena(allocator,
                                 sizeof(Pair));

        pair->key = key;
        pair->value = value;

        pairs[size] = pair;

        object->pairs = pairs;
        object->size = n_size;
    }
    else
    {
        existing_kv_pair->value = value;
    }
}

void json_pop_object(Object *object,
                     char *key)
{
    size_t i;
    Pair *existing_kv_pair = scan_object(object,
                                         key,
                                         &i);

    if (existing_kv_pair)
    {
        const size_t n = object->size - 1 - i;

        memmove(object->pairs + i,
                object->pairs + i + 1,
                sizeof(Pair*) * n);

        object->size--;
    }
}


JSON *json_make_array(void)
{
    JSON *json = NULL;

    if (allocator)
    {
        json = alloc_arena(allocator,
                           sizeof(JSON));

        json->hint = JSON_ARRAY;

        Array *array = alloc_arena(allocator,
                                   sizeof(Array));

        array->values = NULL;
        array->size = 0;

        json->type.a = array;
    }

    return json;
}

void json_push_array(Array *array,
                     JSON *value)
{
    const size_t size = array->size;
    const size_t n_size = size + 1;

    JSON **values = alloc_arena(allocator,
                                sizeof(JSON*) * n_size);

    memcpy(values,
           array->values,
           sizeof(JSON*) * size);

    values[size] = value;

    array->values = values;
    array->size = n_size;
}

void json_pop_array(Array *array,
                    const size_t index)
{
    if (index < array->size)
    {
        const size_t n = array->size - 1 - index;

        memmove(array->values + index,
                array->values + index + 1,
                sizeof(JSON*) * n);

        array->size--;
    }
}
*/

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

        Value *value = parse_value(json,
                                   fdata,
                                   &offset);

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
