#pragma once


#include <stddef.h>


typedef struct Arena Arena;


__attribute__((warn_unused_result))
Arena *create_arena(void);

void destroy_arena(Arena **arena);


__attribute__((warn_unused_result))
void *alloc_arena(Arena *arena,
                  size_t size);

__attribute__((warn_unused_result))
void *calloc_arena(Arena *arena,
                   size_t size);
