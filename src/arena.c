#include <assert.h>
#include <malloc.h>
#include <memory.h>


#define KB(x)     (x * 1024)
#define PAGE_SIZE KB(4)


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


Arena *create_cmantics_arena()
{
    Arena *arena = malloc(sizeof(Arena));

    arena->curr = construct_page(NULL,
                                 1,
                                 PAGE_SIZE);

    return arena;
}

void destroy_cmantics_arena(Arena **arena)
{
    arena_destroy_pages(&(*arena)->curr);

    free(*arena);

    *arena = NULL;
}


void *alloc_cmantics_arena(Arena *arena,
                           const size_t size)
{
    return arena_alloc(&arena->curr,
                       size);
}

void *calloc_cmantics_arena(Arena *arena,
                            size_t size)
{
    void *ptr = alloc_cmantics_arena(arena,
                                     size);

    return memset(ptr,
                  0,
                  size);
}
