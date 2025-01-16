#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../internals/get.h"
#include "../internals/arena.h"
#include "../internals/eval.h"
#include "../internals/obj.h"


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


static Value *parse_json_file(FILE *file)
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

    Value *value = alloc_arena(allocator,
                               sizeof(Value));

    value->hint = HINT_OBJECT,
    value->type.o = make_object(allocator,
                                fdata,
                                &offset);

    free(fdata);

    return value;
}


Value *json_open(const char *path)
{
    FILE *file = fopen(path, "r");

    Value *value = parse_json_file(file);

    fclose(file);

    return value;
}


void json_close(void)
{
    destroy_arena(&allocator);
}


Value *json_find(Value *value,
                 const char *key)
{
    Value *result = NULL;

    if (value->hint == HINT_OBJECT)
    {
        result = scan_object(value->type.o,
                             key);
    }

    return result;
}

Value *json_lookup(Value *value,
                   size_t index)
{
    Value *result = NULL;

    if (value->hint == HINT_ARRAY)
    {
        result = lookup_array(value->type.a,
                              index);
    }

    return result;
}
