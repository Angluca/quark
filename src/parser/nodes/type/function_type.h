#ifndef NODE_FUNCTION_TYPE_H
#define NODE_FUNCTION_TYPE_H

#include "../fields.h"

typedef struct FunctionType {
    TYPE_FIELDS;
    TypeVector signature;
    struct FunctionDeclaration* declaration;
    HashMap(bool) type_definitions;
} FunctionType;

#endif