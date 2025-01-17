#include <assert.h>
#include <stdlib.h>
#include "../internals/write.h"
#include "../internals/get.h"
#include "../internals/eval.h"


static Arena *allocator;


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
    assert(file);

    size_t offset = 0;
    size_t fsize = get_file_size(file);

    allocator = create_arena(fsize * 2);

    char *fdata = calloc(sizeof(char),
                         fsize + 1);

    fread(fdata,
          sizeof(char),
          fsize,
          file);

    enum Eval eval = evaluation(fdata,
                                &offset);

    assert(eval == EVAL_MAKE_OBJ);

    JSON *value = alloc_arena(allocator,
                               sizeof(JSON));

    value->hint = HINT_OBJECT,
    value->type.o = make_object(allocator,
                                fdata,
                                &offset);

    free(fdata);

    return value;
}


JSON *json_open(const char *path)
{
    FILE *file = fopen(path,
                       "r");

    JSON *value = parse_json_file(file);

    fclose(file);

    return value;
}


void json_close(void)
{
    destroy_arena(&allocator);
}


JSON *json_find(JSON *value,
                const char *key)
{
    JSON *result = NULL;

    if (value->hint == HINT_OBJECT)
    {
        result = scan_object(value->type.o,
                             key);
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


void json_write(const char *path,
                const JSON *value)
{
    FILE *file = fopen(path,
                       "w");

    assert(file);

    assert(value->hint == HINT_OBJECT);

    write_object(file,
                 value->type.o,
                 0);

    fclose(file);
}
