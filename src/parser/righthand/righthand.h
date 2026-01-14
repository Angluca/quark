#ifndef RIGHTHAND_H
#define RIGHTHAND_H

#include "../nodes/nodes.h"
#include "../parser.h"

enum {
    RightBinary,
    RightPostfixOrBinary,
    RightDeclaration,
    RightAssignment,
    RightPostfixAssignment,
    RightCall,
    RightFieldAccess,
    RightCompare,
    RightIndex,
    RightOptional,
    RightRange,
};

typedef struct RighthandOperator {
    unsigned char precedence: 4,
            type            : 4;
} RighthandOperator;

extern RighthandOperator global_righthand_operator_table[];
extern bool global_righthand_collecting_type_arguments;

Node* expression(Parser* parser);

Node* righthand_expression(Node* lefthand, Parser* parser, unsigned char precedence);

#endif
