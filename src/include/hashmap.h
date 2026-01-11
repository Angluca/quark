#ifndef HASHMAP_H
#define HASHMAP_H

#include <assert.h>

#include "vector.h"
#include "vector-string.h"

#ifndef MAP_SIZE
#define MAP_SIZE 16
#endif

#define HashMap(V) typeof(Vector(struct { String k; V v; })(*)[MAP_SIZE])

#define put(map, key, value...) do { \
    if(!*(map)) { \
        *(map) = calloc(1, sizeof(**(map))); \
        assert(*(map) && "Out of Memory"); \
    } \
    push(&(**(map))[fnv1a_u32_hash(key) % MAP_SIZE], ((typeof(*(**(map))->data)) { key, value })); \
} while(0)

typedef Vector(void)* hashmap__SearchSection;

void* hashmap__search_section(hashmap__SearchSection section, String key, size_t size);

// #define get(map, key) ((typeof((*(map))->data->v)*) hashmap__get((void*)(map), key, \
//     (map) ? (size_t) &(*(map))[1] - (size_t)(map) : 0))
#define get(map, key) ((typeof((*(map))->data->v)*) hashmap__search_section(\
    (map) ? (void*) &(*(map))[fnv1a_u32_hash(key) % MAP_SIZE] : NULL, key, \
    (map) ? (size_t) (&(*(map))->data[1]) - (size_t) (*(map))->data : 0))

#endif
