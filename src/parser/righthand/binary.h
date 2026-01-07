#ifndef RIGHTHAND_BINARY_H
#define RIGHTHAND_BINARY_H

#include "../parser.h"

Node* try_binary_postfix(Node* lefthand, Parser* parser);

struct RighthandOperator;

Node* parse_binary_operation(Node* lefthand, struct RighthandOperator operator, Parser* parser);

#endif
