#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <memory.h>
#include <malloc.h>
#include "../internals/make.h"
#include "../internals/dict.h"


#define GROW_FACTOR        2.0f
#define SHRINK_FACTOR      0.5f
#define LF_UPPER_THRESHOLD 0.75f
#define LF_LOWER_THRESHOLD 0.1f
#define TABLE_MIN          8

#define UNSET     (-1)
#define INVALID   UNSET
#define NOT_FOUND INVALID


typedef unsigned long Hash;


struct Bucket
{
    size_t cron;
    const char *key;
    void *value;
    bool tombstone;
};


static Hash djb2(const char *key)
{
    Hash hash = 5381;
    const size_t len = strlen(key);

    for (int i = 0; i < len; i++)
    {
        hash = ((hash << 5) + hash) + key[i];
    }

    return hash;
}

static size_t get_index(const Hash hash,
                        const size_t capacity)
{
    return hash % capacity;
}

static bool is_found(const struct Bucket bucket,
                     const void *key,
                     const bool skip_tombstones)
{
    bool found_bucket = !bucket.tombstone &&
                        bucket.key != (const char*)UNSET &&
                        strcmp(key,
                               bucket.key) == 0;

    bool invalid_or_tombstone = !skip_tombstones &&
                                (bucket.key == (const char*)INVALID ||
                                bucket.tombstone);

    return found_bucket ||
           invalid_or_tombstone;
}


static size_t probe(struct Bucket *buckets,
                    const size_t capacity,
                    const void *key,
                    size_t index,
                    const bool skip_tombstones)
{
    assert(buckets);

    size_t found = NOT_FOUND;
    size_t tombstone = NOT_FOUND;
    size_t count = 1;

    while (found == NOT_FOUND)
    {
        struct Bucket bucket = buckets[index];

        if (skip_tombstones)
        {
            if (tombstone == INVALID &&
                bucket.key != (const char*)INVALID &&
                bucket.tombstone)
            {
                tombstone = index;
            }
            else if (bucket.key == (const char*)INVALID)
            {
                break;
            }
        }

        if (is_found(bucket,
                     key,
                     skip_tombstones))
        {
            found = index;
        }
        else
        {
            index = (index + 1) % capacity;
        }

        if (++count > capacity)
        {
            break;
        }
    }

    if (tombstone != INVALID && found != NOT_FOUND)
    {
        buckets[tombstone].tombstone = false;
        buckets[tombstone].value = buckets[found].value;
        buckets[tombstone].key = buckets[found].key;

        buckets[found].tombstone = true;
        buckets[found].value = (void*)UNSET;

        found = tombstone;
    }

    return found;
}


static float get_resize_factor(const size_t nmemb,
                               const size_t capacity,
                               const bool capacity_to_shrink)
{
    float factor = 0;

    if (capacity)
    {
        float load_factor = (float)nmemb / (float)capacity;

        if (load_factor >= LF_UPPER_THRESHOLD)
        {
            factor = GROW_FACTOR;
        }
        else if (load_factor <= LF_LOWER_THRESHOLD && capacity_to_shrink)
        {
            factor = SHRINK_FACTOR;
        }
    }

    return factor;
}


static void initialise_buckets(struct Bucket **buckets,
                               size_t *capacity)
{
    if (*buckets)
    {
        free(*buckets);
    }

    *capacity = TABLE_MIN;
    const size_t size = *capacity * sizeof(struct Bucket);

    *buckets = malloc(size);

    memset(*buckets,
           UNSET,
           size);
}


static void reindex_buckets(struct Bucket *buckets,
                            const struct Bucket *tmp,
                            const size_t old_capacity,
                            const size_t new_capacity)
{
    for (int i = 0; i < old_capacity; i++)
    {
        struct Bucket bucket = tmp[i];

        if (bucket.key == (const char*)INVALID ||
            bucket.tombstone)
        {
            continue;
        }

        size_t index = get_index(djb2(bucket.key),
                                 new_capacity);

        index = probe(buckets,
                      new_capacity,
                      bucket.key,
                      index,
                      false);

        buckets[index] = bucket;
    }
}

static void resize_buckets(struct Bucket **buckets,
                           size_t *capacity,
                           const float factor)
{
    assert(*buckets);

    size_t old_capacity = *capacity;
    size_t new_capacity = (size_t)((float)old_capacity * factor);
    size_t t_size = old_capacity * sizeof(struct Bucket);
    size_t m_size = new_capacity * sizeof(struct Bucket);

    *capacity = new_capacity;

    struct Bucket *tmp = malloc(t_size);

    assert(tmp);

    memcpy(tmp,
           *buckets,
           t_size);

    free(*buckets);

    *buckets = malloc(m_size);

    assert(*buckets);

    memset(*buckets,
           UNSET,
           m_size);

    reindex_buckets(*buckets,
                    tmp,
                    old_capacity,
                    new_capacity);

    free(tmp);
}


static void should_resize(struct Dict *dict)
{
    float resize_factor = get_resize_factor(dict->nmemb,
                                            dict->capacity,
                                            dict->capacity > TABLE_MIN);

    if (!dict->buckets)
    {
        initialise_buckets(&dict->buckets,
                           &dict->capacity);
    }
    else if (resize_factor)
    {
        resize_buckets(&dict->buckets,
                       &dict->capacity,
                       resize_factor);
    }
}


static struct Bucket create_bucket(const size_t cron,
                                   const void *key,
                                   void *value)
{
    struct Bucket bucket = {
        .cron = cron,
        .key = key,
        .value = value,
        .tombstone = false
    };

    return bucket;
}


static size_t find_bucket(struct Bucket *buckets,
                          const size_t capacity,
                          const void *key)
{
    size_t index = NOT_FOUND;

    if (capacity)
    {
        Hash hash = djb2(key);

        index = get_index(hash,
                          capacity);

        index = probe(buckets,
                      capacity,
                      key,
                      index,
                      true);
    }

    return index;
}


struct Dict *create_dict(Arena *arena)
{
    struct Dict *dict = calloc_arena(arena,
                                     sizeof(struct Dict));

    return dict;
}

void destroy_dict(struct Dict *dict)
{
    free(dict->buckets);
}


void insert_dict(struct Dict *dict,
                 const char *key,
                 void *value)
{
    should_resize(dict);

    assert(dict->buckets);

    Hash hash = djb2(key);

    size_t index = get_index(hash,
                             dict->capacity);

    index = probe(dict->buckets,
                  dict->capacity,
                  key,
                  index,
                  false);

    bool exists = dict->buckets[index].key != (const char*)UNSET;

    if (!exists)
    {
        struct Bucket bucket = create_bucket(dict->cron,
                                             key,
                                             value);

        dict->buckets[index] = bucket;
        dict->nmemb++;
        dict->cron++;
    }
    else
    {
        dict->buckets[index].value = value;
    }
}


void erase_dict(struct Dict *dict,
                const void *key)
{
    size_t index = find_bucket(dict->buckets,
                               dict->capacity,
                               key);

    if (index != NOT_FOUND)
    {
        dict->buckets[index].tombstone = true;

        dict->nmemb--;

        should_resize(dict);
    }
}


void *find_dict(struct Dict *dict,
                const void *key)
{
    void *value = NULL;

    if (dict->nmemb)
    {
        size_t index = find_bucket(dict->buckets,
                                   dict->capacity,
                                   key);

        if (index != NOT_FOUND)
        {
            value = dict->buckets[index].value;
        }
    }

    return value;
}


int pair_cmp(const void *a,
             const void *b)
{
    struct Bucket bucket1 = *(struct Bucket*)a;
    struct Bucket bucket2 = *(struct Bucket*)b;

    return (bucket1.cron > bucket2.cron) - (bucket1.cron < bucket2.cron);
}

struct DictPair *get_iterable_pairs(struct Dict *dict,
                                    Arena *arena)
{
    const size_t cpy_size = sizeof(struct Bucket) * dict->capacity;

    struct Bucket *copy = malloc(cpy_size);

    memcpy(copy,
           dict->buckets,
           cpy_size);

    qsort(copy,
          dict->capacity,
          sizeof(struct Bucket),
          pair_cmp);

    struct DictPair *sorted = alloc_arena(arena,
                                          sizeof(struct DictPair) * dict->nmemb);

    for (int i = 0; i < dict->nmemb; i++)
    {
        struct Bucket bucket = copy[i];

        struct DictPair pair = {
            .key = bucket.key,
            .value = bucket.value
        };

        sorted[i] = pair;
    }

    free(copy);

    return sorted;
}
