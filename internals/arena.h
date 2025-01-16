#pragma once

#include <stddef.h>


typedef struct Arena Arena;


__attribute__((warn_unused_result))
Arena *create_arena(size_t size);

void destroy_arena(Arena **arena);


__attribute__((warn_unused_result))
void *alloc_arena(Arena *arena,
                  size_t size);
