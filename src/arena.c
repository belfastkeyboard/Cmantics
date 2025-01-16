#include <assert.h>
#include <malloc.h>
#include "../internals/arena.h"


struct Page
{
    struct Page *prev;
    void *base;
    size_t size;
    size_t offset;
};

typedef struct Arena
{
    struct Page *curr;
} Arena;


static struct Page *construct_page(struct Page *prev,
                                   const size_t growth_policy,
                                   const size_t size)
{
    const size_t n_size = growth_policy * size;

    void *memory = malloc(sizeof(struct Page) + n_size);

    assert(memory);

    struct Page *page = memory;

    page->prev = prev;
    page->base = memory + sizeof(struct Page);
    page->size = n_size;
    page->offset = 0;

    return page;
}

static struct Page *arena_destroy_page(struct Page *page)
{
    struct Page *prev = page->prev;

    free(page);

    return prev;
}

static void arena_destroy_pages(struct Page **page)
{
    do {
        *page = arena_destroy_page(*page);
    } while (*page);
}


static void *arena_alloc(struct Page **curr,
                         const size_t size)
{
    if ((*curr)->size - (*curr)->offset < size)
    {
        *curr = construct_page(*curr,
                               2,
                               (*curr)->size);
    }

    void *ptr = ((*curr)->base + (*curr)->offset);

    (*curr)->offset += size;

    return ptr;
}


Arena *create_arena(const size_t size)
{
    Arena *arena = malloc(sizeof(Arena));

    arena->curr = construct_page(NULL,
                                 1,
                                 size);

    return arena;
}

void destroy_arena(Arena **arena)
{
    arena_destroy_pages(&(*arena)->curr);

    free(*arena);
}


void *alloc_arena(Arena *arena,
                  const size_t size)
{
    return arena_alloc(&arena->curr,
                       size);
}
