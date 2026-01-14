#ifndef SCOPE_H
#define SCOPE_H

#include "../nodes/nodes.h"
#include "../parser.h"

Scope* new_scope(Declaration* parent);

Declaration* find_in_scope_unwrapped(Scope scope, String identifier);

Wrapper* find_in_scope(Scope scope, Trace identifier);

Declaration* find_on_stack_unwrapped(Stack stack, String identifier);

Wrapper* find_on_stack(Stack stack, Trace identifier);

#endif