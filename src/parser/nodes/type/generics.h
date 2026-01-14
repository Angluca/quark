#ifndef NODE_GENERICS_H
#define NODE_GENERICS_H

#include <helpers.h>
#include <hashmap.h>

#include "../fields.h"

typedef Vector(TypeVector) TypeVectorVector;

typedef struct Generics {
    HashMap(UsableVoid) unique_combinations;
    TypeVectorVector type_arguments_stack;
    TypeVector base_type_arguments;
} Generics;

typedef struct GenericReference {
    TYPE_FIELDS;
    Declaration* generics_declaration;
    size_t index;
} GenericReference;

#endif