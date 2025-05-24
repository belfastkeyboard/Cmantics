#pragma once


#include <stddef.h>


typedef struct Arena Arena;


__attribute__((warn_unused_result))
Arena *create_cmantics_arena(void);

void destroy_cmantics_arena(Arena **arena);


__attribute__((warn_unused_result))
void *alloc_cmantics_arena(Arena *arena,
                           const size_t size);

__attribute__((warn_unused_result))
void *calloc_cmantics_arena(Arena *arena,
                            size_t size);
