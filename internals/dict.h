#pragma once


#include <stddef.h>
#include "arena.h"


struct Bucket;


struct DictPair
{
    const char *key;
    void *value;
};

struct Dict
{
    size_t cron;
    struct Bucket *buckets;
    size_t nmemb;
    size_t capacity;
};


struct Dict *create_dict(Arena *arena);

void destroy_dict(struct Dict *dict);


void insert_dict(struct Dict *dict,
                 const char *key,
                 void *value);


void erase_dict(struct Dict *dict,
                const void *key);


void *find_dict(struct Dict *dict,
                const void *key);


struct DictPair *get_iterable_pairs(struct Dict *dict,
                                    Arena *arena);
