#include <malloc.h>
#include <stddef.h>
#include <string.h>
#include "../internals/arena.h"
#include "../internals/make.h"
#include "../internals/dict.h"
#include "../internals/symbols.h"
#include "../ini.h"


struct HeaderNode
{
    const char *name;
    struct Dict *pairs;
};

typedef struct INI
{
    Arena *arena;
    struct Dict *headers;
} INI;


static char *parse_string(char *line,
                          Arena *arena)
{
    const size_t i = strcspn(line,
                             "\n");

    line[i] = '\0';

    char *string = make_string(line,
                               arena);

    return string;
}

static char *get_header_substring(char *line)
{
    const size_t i = strcspn(line + 1,
                             "]");

    line[i + 1] = '\0';

    return line + 1;
}


static void parse_pair(char *line,
                       struct Dict *dict,
                       Arena *arena)
{
    const size_t i = strcspn(line,
                             "=");

    line[i] = '\0';

    const char *key = parse_string(line,
                                   arena);

    const char *value = parse_string(line + i + 1,
                                   arena);

    insert_dict(dict,
                key,
                (void*)value);
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

        parse_pair(line,
                   header->pairs,
                   arena);
    }

    free(line);
}

static struct HeaderNode *create_header(const char *name,
                                        Arena *arena)
{
    struct HeaderNode *header = alloc_arena(arena,
                                            sizeof(struct HeaderNode));

    header->name = name;
    header->pairs = create_dict(arena);

    return header;
}

static struct HeaderNode *parse_header(FILE *file,
                                       char *line,
                                       Arena *arena)
{
    char *substr = get_header_substring(line);

    const char *name = parse_string(substr,
                                    arena);

    struct HeaderNode *header = create_header(name,
                                              arena);

    parse_pairs(header,
                file,
                arena);

    return header;
}


INI *create_ini(void)
{
    Arena *arena = create_arena(PAGE_SIZE);

    INI *ini = alloc_arena(arena,
                           sizeof(INI));

    ini->arena = arena;
    ini->headers = create_dict(arena);

    return ini;
}

void destroy_ini(INI **ini)
{
    if (*ini)
    {
        struct DictPair *pairs = get_iterable_pairs((*ini)->headers,
                                                    (*ini)->arena);

        const size_t count = (*ini)->headers->nmemb;

        for (int i = 0; i < count; ++i)
        {

            struct HeaderNode *header = pairs[i].value;
            destroy_dict(header->pairs);
        }

        destroy_dict((*ini)->headers);

        // valgrind complains if we don't do it this way
        Arena *arena = (*ini)->arena;
        destroy_arena(&arena);

        *ini = NULL;
    }
}


void parse_ini(INI *ini,
               const char *filepath)
{
    FILE *file = fopen(filepath,
                       "r");

    if (ini->headers->nmemb)
    {
        destroy_ini(&ini);
        ini = create_ini();
    }

    if (file)
    {
        char *line = NULL;
        size_t len = 0;

        while (getline(&line,
                       &len,
                       file) != -1)
        {
            if (line[0] == '[')
            {
                struct HeaderNode *header = parse_header(file,
                                                         line,
                                                         ini->arena);

                insert_dict(ini->headers,
                            header->name,
                            header);
            }
        }

        free(line);
    }

    fclose(file);
}

void write_ini(const INI *ini,
               const char *filepath)
{
    if (ini)
    {
        FILE *file = fopen(filepath,
                           "w");

        if (file)
        {
            struct DictPair *headers = get_iterable_pairs(ini->headers,
                                                        ini->arena);

            for (int i = 0; i < ini->headers->nmemb; ++i)
            {
                const char *name = headers[i].key;
                struct HeaderNode *node = headers[i].value;

                fprintf(file,
                        "[%s]\n",
                        name);

                struct DictPair *pairs = get_iterable_pairs(node->pairs,
                                                            ini->arena);

                for (int j = 0; j < node->pairs->nmemb; ++j)
                {
                    const char *k = pairs[j].key;
                    const char *v = pairs[j].value;

                    fprintf(file,
                            "%s=%s\n",
                            k,
                            v);
                }

                if (i < ini->headers->nmemb - 1)
                {
                    fprintf(file,
                            "\n");
                }
            }
        }

        fclose(file);
    }
}


const char *get_ini(const INI *ini,
                    const char *header,
                    const char *key)
{
    const char *value = NULL;

    if (ini)
    {
        struct HeaderNode *node = find_dict(ini->headers,
                                            header);

        if (node)
        {
            value = find_dict(node->pairs,
                              key);
        }
    }

    return value;
}


void set_ini(INI* ini,
             const char *header,
             const char *key,
             const char *value)
{
    if (ini)
    {
        struct HeaderNode *node = find_dict(ini->headers,
                                            header);

        if (!node)
        {
            node = create_header(header,
                                 ini->arena);

            insert_dict(ini->headers,
                        header,
                        node);
        }

        insert_dict(node->pairs,
                    key,
                    (void*)value);
    }
}

void erase_ini(INI* ini,
               const char *header,
               const char *key)
{
    if (ini)
    {
        struct HeaderNode *node = find_dict(ini->headers,
                                            header);

        if (node)
        {
            erase_dict(node->pairs,
                       key);

            if (!node->pairs->nmemb)
            {
                erase_dict(ini->headers,
                           header);
            }
        }
    }
}
