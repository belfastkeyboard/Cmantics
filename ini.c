#include <malloc.h>
#include <stddef.h>
#include <string.h>
#include "arena.h"
#include "ini.h"


struct Pair
{
    const char *key;
    const char *value;
};

struct HeaderNode
{
    const char *name;
    struct Pair *pairs;
    size_t count;
};

typedef struct INI
{
    Arena *arena;
    struct HeaderNode *headers;
    size_t count;
} INI;


static void *bump_array(void *array,
                        const size_t size,
                        const size_t nmemb)
{
    void *temp = realloc(array,
                         size * nmemb);

    if (temp)
    {
        array = temp;
    }

    return array;
}


static char *make_string(char *line,
                         Arena *arena)
{
    const size_t i = strcspn(line,
                             "\n");

    line[i] = '\0';

    const size_t len = strlen(line);

    char *string = alloc_arena(arena,
                               len + 1);

    memset(string,
           0,
           len + 1);

    strncpy(string,
            line,
            len);

    return string;
}

static char *get_header_substring(char *line)
{
    const size_t i = strcspn(line + 1,
                             "]");

    line[i + 1] = '\0';

    return line + 1;
}


static struct Pair parse_pair(char *line,
                              Arena *arena)
{
    const size_t i = strcspn(line,
                             "=");

    line[i] = '\n';

    struct Pair pair = {
        .key = make_string(line,
                           arena),
        .value = make_string(line + i + 1,
                             arena)
    };

    return pair;
}

static void parse_pairs(struct HeaderNode *header,
                        FILE *file,
                        Arena *arena)
{
    char *line = NULL;
    size_t len = 0;

    while (getline(&line,
                   &len,
                   file) != -1)
    {
        if (line[0] == '\n')
        {
            break;
        }

        header->pairs = bump_array(header->pairs,
                                   sizeof(struct Pair),
                                   ++header->count);

        header->pairs[header->count - 1] = parse_pair(line,
                                                        arena);
    }

    free(line);
}

static struct HeaderNode parse_header(FILE *file,
                                      char *line,
                                      Arena *arena)
{
    char *substr = get_header_substring(line);

    const char *name = make_string(substr,
                                   arena);

    struct HeaderNode header = {
        .name = name,
        .pairs = NULL,
        .count = 0
    };

    parse_pairs(&header,
                  file,
                  arena);

    return header;
}


INI *parse_ini(const char *filepath)
{
    INI *ini = NULL;

    FILE *file = fopen(filepath,
                       "r");

    if (file)
    {
        ini = malloc(sizeof(INI));

        ini->arena = create_arena(4096);
        ini->headers = NULL;
        ini->count = 0;

        char *line = NULL;
        size_t len = 0;

        while (getline(&line,
                       &len,
                       file) != -1)
        {
            if (line[0] == '[')
            {
                ini->headers = bump_array(ini->headers,
                                          sizeof(struct HeaderNode),
                                          ++ini->count);

                ini->headers[ini->count - 1] = parse_header(file,
                                                              line,
                                                              ini->arena);
            }
        }

        free(line);
    }

    fclose(file);

    return ini;
}

void destroy_ini(INI **ini)
{
    if (*ini)
    {
        const size_t headers = (*ini)->count;

        for (int i = 0; i < headers; i++)
        {
            struct HeaderNode header = (*ini)->headers[i];

            free(header.pairs);
        }

        free((*ini)->headers);

        destroy_arena(&(*ini)->arena);

        free(*ini);

        *ini = NULL;
    }
}


const char *get_value(INI *ini,
                      const char *header,
                      const char *key)
{
    const char *value = NULL;

    if (ini)
    {
        for (int i = 0; i < ini->count; ++i)
        {
            struct HeaderNode *node = ini->headers + i;

            if (strcmp(header,
                       node->name) == 0)
            {
                for (int j = 0; j < node->count; ++j)
                {
                    struct Pair *pair = node->pairs + j;

                    if (strcmp(key,
                               pair->key) == 0)
                    {
                        value = pair->value;

                        break;
                    }
                }

                break;
            }
        }
    }

    return value;
}
