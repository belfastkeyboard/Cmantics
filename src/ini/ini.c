#include <malloc.h>
#include <stddef.h>
#include "../../internals/arena.h"
#include "../../internals/array.h"
#include "../../internals/dict.h"
#include "../../internals/ini/parse.h"
#include "../../internals/ini/section.h"
#include "../../internals/ini/write.h"
#include "../../ini.h"


typedef struct INI
{
    Arena *arena;
    Array *meta;
    struct Dict *sections;
} INI;


INI *create_ini(void)
{
    Arena *arena = create_arena();

    INI *ini = alloc_arena(arena,
                           sizeof(INI));

    ini->arena = arena;
    ini->meta = create_array(arena);
    ini->sections = create_dict(arena,
                                false);

    return ini;
}

void destroy_ini(INI **ini)
{
    if (*ini)
    {
        struct DictPair *pairs = get_iterable_pairs((*ini)->sections,
                                                    (*ini)->arena);

        const size_t count = (*ini)->sections->nmemb;

        for (int i = 0; i < count; ++i)
        {

            struct Section *header = pairs[i].value;
            destroy_dict(header->pairs);
        }

        destroy_dict((*ini)->sections);

        // TODO: destroy meta

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

    if (ini->sections->nmemb)
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
                struct Section *header = parse_section(file,
                                                       line,
                                                       ini->meta,
                                                       ini->arena);

                insert_dict(ini->sections,
                            header->name,
                            header);
            }
        }

        free(line);

        fclose(file);
    }
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
            struct DictPair *headers = get_iterable_pairs(ini->sections,
                                                          ini->arena);


            for (int i = 0; i < ini->sections->nmemb; ++i)
            {
                struct DictPair header = headers[i];

                write_section(file,
                              header,
                              ini->arena);

                if (i < ini->sections->nmemb - 1)
                {
                    fprintf(file,
                            "\n");
                }
            }
        }

        fclose(file);
    }
}


ValueINI *get_ini(const INI *ini,
                  const char *section,
                  const char *key)
{
    ValueINI *value = NULL;

    if (ini)
    {
        struct Section *node = find_dict(ini->sections,
                                         section);

        if (node)
        {
            value = find_dict(node->pairs,
                              key);
        }
    }

    return value;
}


void set_ini(INI* ini,
             const char *section,
             const char *key,
             const char *value)
{
    if (ini)
    {
        struct Section *sec = find_dict(ini->sections,
                                        section);

        if (!sec)
        {
            sec = create_section(section,
                                 ini->arena);

            insert_dict(ini->sections,
                        section,
                        sec);
        }

        insert_dict(sec->pairs,
                    key,
                    (void*)value);
    }
}

void erase_ini(INI* ini,
               const char *section,
               const char *key)
{
    if (ini)
    {
        struct Section *sec = find_dict(ini->sections,
                                        section);

        if (sec)
        {
            erase_dict(sec->pairs,
                       key);

            if (!sec->pairs->nmemb)
            {
                erase_dict(ini->sections,
                           section);
            }
        }
    }
}
