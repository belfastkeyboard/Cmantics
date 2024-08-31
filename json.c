#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "arena.h"
#include "json.h"

// decl
typedef enum eval_t eval_t;
typedef struct Array Array;
typedef struct Key Key;
typedef struct Value Value;
typedef struct Pair Pair;

static void throw(const char *message);
static size_t count_children(char *data, const size_t *offset);
static size_t count_array(char *data, const size_t *offset);
static size_t get_file_size(FILE* file);
static eval_t evaluation(char *data, size_t *offset);
static value_t determine_type(char c);
static value_t determine_digit(char *data, const size_t *offset);
static void handle_string(Arena *arena, char **src, char *data, size_t *offset);
static int handle_integer(char *data, size_t *offset);
static float handle_float(char *data, size_t *offset);
static bool handle_boolean(char *data, char c, size_t *offset);
static void *handle_null(char *data, size_t *offset);
static Array make_array(Arena *arena, char *data, size_t *offset);
static Pair make_pair(Arena *arena, char *data, size_t *offset);
static Object make_object(Arena *arena, char *data, size_t *offset);
static JSON *parse_json_file(JSON *json, FILE *file);
static Value *traverse_object(Object *object, Value *value, const char *key);
static Value *traverse_array(Array *array, Value *value, const char *key);
// decl

typedef enum eval_t
{
    EVAL_ERROR, EVAL_MAKE_OBJ, EVAL_FIN_OBJ, EVAL_MAKE_PAIR
} eval_t;

typedef struct Key
{
    char *string;
} Key;
typedef struct Pair
{
    Key key;
    Value value;
} Pair;
typedef struct JSON
{
    Object root;
    Arena *arena;
} JSON;

static void throw(const char *message)
{
    printf("%s.\n", message);
    exit(1);
}

static size_t count_children(char *data, const size_t *offset)
{
    char *cpy = data + *offset;

    size_t count = 0;
    size_t pos = strspn(cpy, " \n");

    if (cpy[pos] != '}')
    {
        size_t depth = 0;
        size_t len = strlen(cpy);
        bool in_string = false;

        for (int i = 0; i < len; i++)
        {
            char c = cpy[i];

            if (!in_string && c == ':' && depth == 0)
                count++;
            else if (!in_string && c == '{' || c == '[')
                depth++;
            else if (!in_string && depth > 0 && (c == '}' || c == ']'))
                depth--;
            else if (depth == 0 && c == '"')
                in_string = !in_string;
            else if (!in_string && depth == 0 && c == '}')
                break;
        }
    }

    return count;
}
static size_t count_array(char *data, const size_t *offset)
{
    size_t count = 0;
    size_t pos = strspn(data + *offset, " \n");

    if (data[pos] != '}')
    {
        size_t depth = 0;
        size_t len = strlen(data);
        bool in_string = false;

        for (int i = 0; i < len; i++)
        {
            char c = data[*offset + i];

            if (!in_string && c == ',' && depth == 0)
                count++;
            else if (!in_string && c == '{' || c == '[')
                depth++;
            else if (!in_string && depth > 0 && (c == '}' || c == ']'))
                depth--;
            else if (depth == 0 && c == '"')
                in_string = !in_string;
            else if (!in_string && depth == 0 && c == ']')
                break;
        }

        count++; // count the last item
    }

    return count;
}
static size_t get_file_size(FILE* file)
{
    ssize_t fsize;

    rewind(file);
    fseek(file, 0, SEEK_END);
    fsize = ftell(file);
    rewind(file);

    return fsize;
}

static eval_t evaluation(char *data, size_t *offset)
{
    eval_t eval;
    *offset += strcspn(data + *offset, "{}[]\"");

    char c = *(data + *offset);
    switch (c)
    {
        case '{':
            eval = EVAL_MAKE_OBJ;
            break;
        case '}':
            eval = EVAL_FIN_OBJ;
            break;
        case '"':
            eval = EVAL_MAKE_PAIR;
            break;
        default:
            eval = EVAL_ERROR;
            break;
    }

    (*offset)++;

    return eval;
}
static value_t determine_type(const char c)
{
    value_t value;

    if (c == '"')
        value = VALUE_STRING;
    else if (isdigit(c) || c == '-')
        value = VALUE_DIGIT;
    else if (c == 'f' || c == 't')
        value = VALUE_BOOL;
    else if (c == '[')
        value = VALUE_ARRAY;
    else if (c == '{')
        value = VALUE_OBJECT;
    else if (c == 'n')
        value = VALUE_NULL;
    else
        value = VALUE_ERROR;

    return value;
}
static value_t determine_digit(char *data, const size_t *offset)
{
    value_t type = VALUE_INT;

    size_t len = strspn(data + *offset, "-0123456789.");
    for (int i = 0; i < len; i++)
    {
        if (data[i] == '.')
        {
            type = VALUE_FLOAT;
            break;
        }
    }

    return type;
}

static void handle_string(Arena *arena, char **src, char *data, size_t *offset)
{
    size_t len = strcspn(data + *offset, "\"");

    *src = calloc_arena(arena, len + 1);
    strncpy(*src, data + *offset, len);

    *offset += len + 1;
}
static int handle_integer(char *data, size_t *offset)
{
    int result;
    char *ptr;

    result = (int)strtol(data + *offset, &ptr, 10);
    *offset += ptr - (data + *offset);

    return result;
}
static float handle_float(char *data, size_t *offset)
{
    float result;
    char *ptr;

    result = strtof(data + *offset, &ptr);
    *offset += ptr - (data + *offset);

    return result;
}
static bool handle_boolean(char *data, char c, size_t *offset)
{
    char boolean[6] = { 0 };

    size_t len = (c == 'f') ? 5 : 4;
    strncpy(boolean, data + *offset, len);

    *offset += len;

    return (strcmp(boolean, "true") == 0);
}
static void *handle_null(char *data, size_t *offset)
{
    char null[5] = { 0 };

    size_t len = 4;
    strncpy(null, data + *offset, len);

    if (strcmp(null, "null") != 0)
        throw("handle_null() error");

    *offset += len;

    return NULL;
}

static Array make_array(Arena *arena, char *data, size_t *offset)
{
    Array array = { 0 };

    array.size = count_array(data, offset);
    array.hints = alloc_arena(arena, sizeof(value_t) * array.size);
    array.values = alloc_arena(arena, sizeof(json_t) * array.size);

    *offset += strspn(data + *offset, " \n");

    for (int i = 0; i < array.size; i++)
    {
        value_t hint;
        json_t  value;
        char c = data[*offset];
        value_t type = determine_type(c);

        if (type == VALUE_DIGIT)
        {
            if (determine_digit(data, offset) == VALUE_INT)
            {
                hint = VALUE_INT;
                value.i = handle_integer(data, offset);
            }
            else
            {
                hint = VALUE_FLOAT;
                value.f = handle_float(data, offset);
            }
        }
        else if (type == VALUE_BOOL)
        {
            hint = type;
            value.b = handle_boolean(data, c, offset);
        }
        else if (type == VALUE_STRING)
        {
            (*offset)++;
            hint = type;
            value.s = NULL;
            handle_string(arena, &value.s, data, offset);
        }
        else if (type == VALUE_OBJECT)
        {
            (*offset)++;
            hint = type;
            value.o = make_object(arena, data, offset);
        }
        else if (type == VALUE_ARRAY)
        {
            (*offset)++;
            hint = type;
            value.a = make_array(arena, data, offset);
        }
        else
        {
            hint = type;
            value.n = handle_null(data, offset);
        }

        array.hints[i] = hint;
        array.values[i] = value;

        *offset += strspn(data + *offset, ", \n");
    }

    *offset += strspn(data + *offset, "]") + 1;

    return array;
}
static Pair make_pair(Arena *arena, char *data, size_t *offset)
{
    Pair pair = { 0 };

    handle_string(arena, &pair.key.string, data, offset);
    *offset += strspn(data + *offset, ": ");

    char c = data[*offset];
    value_t type = determine_type(c);

    if (type == VALUE_STRING)
    {
        (*offset)++;
        pair.value.type.s = NULL;
        handle_string(arena, &pair.value.type.s, data, offset);
    }
    else if (type == VALUE_DIGIT)
    {
        if (determine_digit(data, offset) == VALUE_INT)
        {
            type = VALUE_INT;
            pair.value.type.i = handle_integer(data, offset);
        }
        else
        {
            type = VALUE_FLOAT;
            pair.value.type.f = handle_float(data, offset);
        }
    }
    else if (type == VALUE_BOOL)
    {
        pair.value.type.b = handle_boolean(data, c, offset);
    }
    else if (type == VALUE_ARRAY)
    {
        (*offset)++;
        pair.value.type.a = make_array(arena, data, offset);
    }
    else if (type == VALUE_OBJECT)
    {
        (*offset)++;
        pair.value.type.o = make_object(arena, data, offset);
    }
    else if (type == VALUE_NULL)
    {
        pair.value.value_enum = type;
        pair.value.type.n = handle_null(data, offset);
    }
    else
        throw("make_pair() error");

    pair.value.value_enum = type;

    return pair;
}
static Object make_object(Arena *arena, char *data, size_t *offset)
{
    Object object = { 0 };
    eval_t eval;

    size_t child_count = count_children(data, offset);
    object.kv_pairs = alloc_arena(arena, sizeof(Pair) * child_count);

    while ((eval = evaluation(data, offset)) != EVAL_FIN_OBJ)
    {
        if (eval == EVAL_MAKE_PAIR)
        {
            object.kv_pairs[object.pair_size] = make_pair(arena, data, offset);
            object.pair_size++;
        }
    }

    return object;
}

static JSON *parse_json_file(JSON *json, FILE *file)
{
    if (!json)
        throw("JSON is NULL");
    else
    {
        char *fdata;
        size_t fsize;
        size_t offset = 0;

        fsize = get_file_size(file);

        json->arena = create_arena(fsize * 2, ARENA_DYNAMIC);
        fdata = calloc_arena(json->arena, fsize + 1);

        fread(fdata, sizeof(char), fsize, file);

        eval_t eval = evaluation(fdata, &offset);

        if (eval == EVAL_MAKE_OBJ)
            json->root = make_object(json->arena, fdata, &offset);
    }

    return json;
}
static Value *traverse_array(Array *array, Value *value, const char *key)
{
    for (int i = 0; i < array->size && !value; i++)
    {
        value_t hint = array->hints[i];
        json_t type = array->values[i];

        if (hint == VALUE_OBJECT)
            value = traverse_object(&type.o, value, key);
    }

    return value;
}
static Value *traverse_object(Object *object, Value *value, const char *key)
{
    for (int i = 0; i < object->pair_size && !value; i++)
    {
        Pair *pair = &object->kv_pairs[i];

        if (strcmp(key, pair->key.string) == 0)
            value = &pair->value;
        else if (pair->value.value_enum == VALUE_ARRAY)
            value = traverse_array(&pair->value.type.a, value, key);
        else if (pair->value.value_enum == VALUE_OBJECT)
            value = traverse_object(&pair->value.type.o, value, key);
    }

    return value;
}

JSON *json_fopen(const char *path)
{
    JSON *json = malloc(sizeof(JSON));

    FILE *file = fopen(path, "r");
    json = parse_json_file(json, file);
    fclose(file);

    return json;
}
void json_fclose(JSON **json)
{
    destroy_arena(&(*json)->arena);

    free(*json);
    *json = NULL;
}
Value *json_find(JSON *json, const char *key)
{
    Value *value = NULL;

    value = traverse_object(&json->root, value, key);

    return value;
}
