#include "hashmap.h"

void* hashmap__search_section(const hashmap__SearchSection section, const String key, const size_t size) {
    if(section) for(size_t i = 0; i < section->size; i++) {
        if(streq(*(String*)(section->data + i * size), key)) {
            return section->data + i * size + sizeof(String);
        }
    }
    return NULL;
}
