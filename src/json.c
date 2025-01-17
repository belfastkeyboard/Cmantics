#include <assert.h>
#include <errno.h>
#include <memory.h>
#include <stdlib.h>
#include "../internals/array.h"
#include "../internals/ext.h"
#include "../internals/eval.h"
#include "../internals/get.h"
#include "../internals/write.h"
#include "../internals/value.h"


Arena *allocator;


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


static JSON *parse_json_file(FILE *file)
{
    size_t offset = 0;
    size_t fsize = get_file_size(file);

    char *fdata = calloc(sizeof(char),
                         fsize + 1);

    fread(fdata,
          sizeof(char),
          fsize,
          file);

    JSON *value = make_value(fdata,
                             &offset);

    free(fdata);

    return value;
}


void json_open(void)
{
    allocator = create_arena(4096);
}


void json_close(void)
{
    if (allocator)
    {
        destroy_arena(&allocator);
    }
}


JSON *json_find(JSON *value,
                const char *key)
{
    JSON *result = NULL;

    if (value->hint == HINT_OBJECT)
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

JSON *json_lookup(JSON *value,
                  size_t index)
{
    JSON *result = NULL;

    if (value->hint == HINT_ARRAY)
    {
        result = lookup_array(value->type.a,
                              index);
    }

    return result;
}


JSON *json_make_value(const Hint hint,
                      void *type)
{

    JSON *json = NULL;

    if (hint != HINT_OBJECT &&
        hint != HINT_ARRAY)
    {
        json = alloc_arena(allocator,
                           sizeof(JSON));

        json->hint = hint;
    }

    switch (hint)
    {
        case HINT_INT:
            json->type.i = *(int*)type;
            break;
        case HINT_FLOAT:
            json->type.f = *(float*)type;
            break;
        case HINT_NULL:
            json->type.n = NULL;
            break;
        case HINT_BOOL:
            json->type.b = *(bool*)type;
            break;
        case HINT_STRING:
            json->type.s = (char*)type;
            break;
        case HINT_ARRAY:
            json = json_make_array();
            break;
        case HINT_OBJECT:
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

        json->hint = HINT_OBJECT;

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

        json->hint = HINT_ARRAY;

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


JSON *json_read(const char *path)
{
    JSON *json = NULL;

    if (is_json_file(path))
    {
        FILE *file = fopen(path,
                           "r");

        if (file)
        {
            json = parse_json_file(file);

            fclose(file);
        }
    }
    else
    {
        errno = EINVAL;
    }

    return json;
}

void json_write(const char *path,
                const JSON *value)
{
    FILE *file = fopen(path,
                       "w");

    assert(file);

    write_value(file,
                value,
                0);

    fclose(file);
}
