#ifndef NODE_IDENTIFIER_H
#define NODE_IDENTIFIER_H

#include "../../fields.h"

typedef struct Identifier {
    String base;
    Declaration* parent_scope;
    Declaration* parent_declaration;
    unsigned function_declaration_counter;
    bool is_external : 1;
} Identifier;

#endif