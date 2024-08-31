
#include <stdbool.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"

// decl
typedef enum eval_t eval_t;
typedef struct Array Array;
typedef struct Key Key;
typedef struct Value Value;
typedef struct Pair Pair;

static void throw(const char *message);
static size_t count_children(char *data);
static size_t count_array(char *data);
static size_t get_file_size(FILE* file);
static eval_t evaluation(char **ptr);
static value_t determine_type(char c);
static value_t determine_digit(char *data);
static void handle_string(char **src, char **ptr);
static int handle_integer(char **ptr);
static float handle_float(char **ptr);
static bool handle_boolean(char **ptr, char c);
static void *handle_null(char **ptr);
static Array make_array(char **ptr);
static Pair make_pair(char **ptr);
static Object make_object(char **ptr);
static JSON *parse_json_file(JSON *json);
static Value *traverse_object(Object *object, Value *value, const char *key);
static Value *traverse_array(Array *array, Value *value, const char *key);
// decl

typedef enum eval_t
{
    EVAL_ERROR, EVAL_MAKE_OBJ, EVAL_FIN_OBJ, EVAL_MAKE_LIST, EVAL_FIN_LIST, EVAL_MAKE_PAIR
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
    FILE *file;
    Object root;
} JSON;

static void throw(const char *message)
{
    printf("%s.\n", message);
    exit(1);
}

static size_t count_children(char *data)
{
    size_t count = 0;
    size_t pos = strspn(data, " \n");

    if (data[pos] != '}')
    {
        size_t depth = 0;
        size_t len = strlen(data);
        bool in_string = false;

        for (int i = 0; i < len; i++)
        {
            char c = data[i];

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
static size_t count_array(char *data)
{
    size_t count = 0;
    size_t pos = strspn(data, " \n");

    if (data[pos] != '}')
    {
        size_t depth = 0;
        size_t len = strlen(data);
        bool in_string = false;

        for (int i = 0; i < len; i++)
        {
            char c = data[i];

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

static eval_t evaluation(char **ptr)
{
    eval_t eval;
    char *data = *ptr;
    size_t pos = strcspn(data, "{}[]\"");

    data = &data[pos];

    switch (*data)
    {
        case '{':
            eval = EVAL_MAKE_OBJ;
            break;
        case '}':
            eval = EVAL_FIN_OBJ;
            break;
        case '[':
            eval = EVAL_MAKE_LIST;
            break;
        case ']':
            eval = EVAL_FIN_LIST;
            break;
        case '"':
            eval = EVAL_MAKE_PAIR;
            break;
        default:
            eval = EVAL_ERROR;
            break;
    }

    *ptr = ++data;

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
static value_t determine_digit(char *data)
{
    value_t type = VALUE_INT;

    size_t len = strcspn(data, ",");
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

static void handle_string(char **src, char **ptr)
{
    char *data = *ptr;

    size_t len = strcspn(data, "\"");

    *src = calloc(len + 1, sizeof(char));
    strncpy(*src, data, len);

    data = &data[len + 1];

    *ptr = data;
}
static int handle_integer(char **ptr)
{
    int result;

    char *data = *ptr;

    result = (int)strtol(data, ptr, 10);
    (*ptr)++;

    return result;
}
static float handle_float(char **ptr)
{
    float result;

    char *data = *ptr;

    result = strtof(data, ptr);
    (*ptr)++;

    return result;
}
static bool handle_boolean(char **ptr, char c)
{
    char boolean[6] = { 0 };
    char *data = *ptr;

    size_t len = (c == 'f') ? 5 : 4;
    strncpy(boolean, data, len);

    *ptr = &data[len];

    return (strcmp(boolean, "true") == 0);
}
static void *handle_null(char **ptr)
{
    char null[5] = { 0 };
    char *data = *ptr;

    size_t len = 4;
    strncpy(null, data, len);

    if (strcmp(null, "null") != 0)
        throw("handle_null() error");

    *ptr = &data[len];

    return NULL;
}

static Array make_array(char **ptr)
{
    Array array = { 0 };

    array.size = count_array(*ptr);
    array.hints = malloc(sizeof(value_t) * array.size);
    array.values = malloc(sizeof(json_t) * array.size);

    *ptr = &(*ptr)[strspn(*ptr, " \n")];

    for (int i = 0; i < array.size; i++)
    {
        value_t hint;
        json_t  value;
        char c = (*ptr)[0];
        value_t type = determine_type(c);

        if (type == VALUE_DIGIT)
        {
            if (determine_digit(*ptr) == VALUE_INT)
            {
                hint = VALUE_INT;
                value.i = handle_integer(ptr);
            }
            else
            {
                hint = VALUE_FLOAT;
                value.f = handle_float(ptr);
            }
        }
        else if (type == VALUE_BOOL)
        {
            hint = type;
            value.b = handle_boolean(ptr, c);
        }
        else if (type == VALUE_STRING)
        {
            (*ptr)++;
            hint = type;
            value.s = NULL;
            handle_string(&value.s, ptr);
        }
        else if (type == VALUE_OBJECT)
        {
            (*ptr)++;
            hint = type;
            value.o = make_object(ptr);
        }
        else if (type == VALUE_ARRAY)
        {
            (*ptr)++;
            hint = type;
            value.a = make_array(ptr);
        }
        else
        {
            hint = type;
            value.n = handle_null(ptr);
        }

        array.hints[i] = hint;
        array.values[i] = value;

        *ptr = &(*ptr)[strspn(*ptr, ", \n")];
    }

    *ptr = &(*ptr)[strcspn(*ptr, "]") + 1];

    return array;
}
static Pair make_pair(char **ptr)
{
    Pair pair = { 0 };

    handle_string(&pair.key.string, ptr);
    *ptr = &(*ptr)[strspn(*ptr, ": ")];

    char c = (*ptr)[0];
    value_t type = determine_type(c);

    if (type == VALUE_STRING)
    {
        (*ptr)++;
        pair.value.type.s = NULL;
        handle_string(&pair.value.type.s, ptr);
    }
    else if (type == VALUE_DIGIT)
    {
        if (determine_digit(*ptr) == VALUE_INT)
        {
            type = VALUE_INT;
            pair.value.type.i = handle_integer(ptr);
        }
        else
        {
            type = VALUE_FLOAT;
            pair.value.type.f = handle_float(ptr);
        }
    }
    else if (type == VALUE_BOOL)
    {
        pair.value.type.b = handle_boolean(ptr, c);
    }
    else if (type == VALUE_ARRAY)
    {
        (*ptr)++;
        pair.value.type.a = make_array(ptr);
    }
    else if (type == VALUE_OBJECT)
    {
        (*ptr)++;
        pair.value.type.o = make_object(ptr);
    }
    else if (type == VALUE_NULL)
    {
        pair.value.value_enum = type;
        pair.value.type.n = handle_null(ptr);
    }
    else
        throw("make_pair() error");

    pair.value.value_enum = type;

    return pair;
}
static Object make_object(char **ptr)
{
    Object object = { 0 };
    eval_t eval;

    size_t child_count = count_children(*ptr);
    object.kv_pairs = malloc(sizeof(Pair) * child_count);

    while ((eval = evaluation(ptr)) != EVAL_FIN_OBJ)
    {
        if (eval == EVAL_MAKE_PAIR)
        {
            object.kv_pairs[object.pair_size] = make_pair(ptr);
            object.pair_size++;
        }
        else
        {
            // throw an error
        }
    }

    return object;
}

static JSON *parse_json_file(JSON *json)
{
    char *fdata;
    size_t fsize;

    fsize = get_file_size(json->file);
    fdata = malloc(fsize + 1);

    char *cpy = fdata;

    fread(cpy, sizeof(char), fsize, json->file);

    eval_t eval = evaluation(&cpy);

    if (eval == EVAL_MAKE_OBJ)
        json->root = make_object(&cpy);

    free(fdata);

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

JSON *json_fopen(const char *file)
{
    JSON *json = malloc(sizeof(JSON));

    json->file = fopen(file, "r");
    json = parse_json_file(json);

    return json;
}
void json_fclose(JSON *json)
{
    fclose(json->file);

    json->file = NULL;
}
Value *json_find(JSON *json, const char *key)
{
    Value *value = NULL;

    value = traverse_object(&json->root, value, key);

    return value;
}
