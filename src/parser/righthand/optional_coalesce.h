#ifndef OPTIONAL_COALESCE_H
#define OPTIONAL_COALESCE_H

#include "../nodes/nodes.h"
#include "../parser.h"

Node* parse_optional_coalescing(Node* lefthand, Parser* parser);

#endif