#include "scope.h"
#include "../literal/wrapper.h"

Scope* new_scope(Declaration* const parent) {
    Scope* const scope = (void*) new_node((Node) {
        .Scope = {
            .id = NodeScope,
            .parent = parent,
        }
    });

    if(!scope->parent) scope->parent = (void*) scope;
    return scope;
}

Declaration* find_in_scope_unwrapped(const Scope scope, const String identifier) {
    Declaration** const possible_found = get(scope.variables, identifier);
    return possible_found ? *possible_found : NULL;
}

Wrapper* find_in_scope(const Scope scope, const Trace identifier) {
    Declaration** const possible_found = get(scope.variables, identifier.source);
    return possible_found ? variable_of(*possible_found, identifier, 0) : NULL;
}

Declaration* find_on_stack_unwrapped(const Stack stack, const String identifier) {
    for(size_t i = stack.size; i > 0; i--) {
        Declaration* possible_found = find_in_scope_unwrapped(*stack.data[i - 1], identifier);
        if(possible_found) return possible_found;
    }
    return NULL;
}

Wrapper* find_on_stack(const Stack stack, const Trace identifier) {
    for(size_t i = stack.size; i > 0; i--) {
        Wrapper* possible_found = find_in_scope(*stack.data[i - 1], identifier);
        if(possible_found) return possible_found;
    }
    return NULL;
}