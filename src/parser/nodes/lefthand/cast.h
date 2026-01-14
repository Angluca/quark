#ifndef NODE_CAST_H
#define NODE_CAST_H

#include "../fields.h"

typedef struct Cast {
    NODE_FIELDS;
    Node* value;
} Cast;

#endif