#include "function.h"

#include "identifier.h"
#include "../../statement/scope.h"
#include "../../type/types.h"
#include "../../type/generics.h"
#include "../../statement/statement.h"
#include "../righthand.h"
#include "../../literal/wrapper.h"

static int recycle_missing_generics(Type* missing, Type* ignore, void* void_parser) {
    (void) ignore;
    Parser* const parser = void_parser;

    if(missing->id != NodeMissing) return 0;
    Wrapper* possible_found = find_on_stack(parser->stack, missing->trace);

    if(possible_found && possible_found->flags & fType) {
        *missing = *(Type*) (void*) possible_found;
        unbox((void*) possible_found);
        return 1;
    }

    unbox((void*) possible_found);
    return 0;
}

Node* parse_function_declaration(Type* return_type, IdentifierInfo info, Parser* parser) {
    Trace trace_start = stretch(return_type->trace, info.trace);

    FunctionType* function_type = (void*) new_type((Type) {
        .FunctionType = {
            .id = NodeFunctionType,
            .trace = trace_start,
        }
    });
    push(&function_type->signature, return_type);

    FunctionDeclaration* declaration = (void*) new_node((Node) {
        .FunctionDeclaration = {
            .id = NodeFunctionDeclaration,
            .flags = info.identifier.is_external,
            .trace = trace_start,
            .type = (void*) function_type,
            .identifier = info.identifier,
        }
    });
    declaration->body = info.generics_collection.generic_declarations_scope ? : new_scope(NULL);
    declaration->body->parent = (void*) declaration;

    function_type->declaration = declaration;
    info.identifier.parent_declaration = (void*) declaration;

    assign_generics_to_declaration((void*) declaration, info.generics_collection);
    push(&parser->stack, declaration->body);
    traverse_type(return_type, NULL, &recycle_missing_generics, parser, TraverseGenerics);

    push(&info.declaration_scope->hoisted_declarations, (void*) declaration);
    put(&info.declaration_scope->variables, info.identifier.base, (void*) declaration);

    const NodeVector argument_declarations = collect_until(parser, &expression, ',', ')');

    for(size_t i = 0; i < argument_declarations.size; i++) {
        Node* const argument = argument_declarations.data[i];

        if(argument->id == WrapperVariable && argument->flags & fIgnoreStatement) {
            push(&function_type->signature, argument->type);

            const Argument argument_value = {
                .type = argument->type,
                .identifier = argument->Wrapper.Variable.declaration->identifier.base,
            };

            push(&declaration->arguments, argument_value);

            unbox(argument);
            continue;
        }

        push(parser->tokenizer->messages,
             REPORT_ERR(argument_declarations.data[i]->trace, String("expected an argument declaration")));
        unbox(argument);
    }
    free(argument_declarations.data);

    if(!(declaration->flags & fExternal)) {
        expect(parser->tokenizer, '{');
        declaration->body->children = collect_until(parser, &statement, 0, '}');
    }

    close_generics_declaration((void*) declaration);
    pop(&parser->stack);

    return (void*) variable_of((void*) declaration, declaration->trace,
                               fIgnoreStatement | fStatementTerminated * !(declaration->flags & fExternal));
}
