#ifndef LITERAL_ARRAY_H
#define LITERAL_ARRAY_H

#include "../nodes/nodes.h"
#include "../parser.h"

Declaration* fetch_slice_declaration(Parser* parser);

Node* parse_array_literal(Trace trace_start, Parser* parser);

#endif