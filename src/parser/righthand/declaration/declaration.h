#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#include "../../nodes/nodes.h"
#include "../../parser.h"

Message see_declaration(Declaration* declaration, Trace trace);

Node* parse_declaration(Node* type, Token identifier, Parser* parser);

#endif
