#include "function.h"

#include "identifier.h"
#include "../../statement/scope.h"
#include "../../type/types.h"
#include "../../type/generics.h"
#include "../../statement/statement.h"
#include "../righthand.h"
#include "../../literal/wrapper.h"
#include "parser/lefthand/lefthand.h"

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
    const Trace trace_start = stretch(return_type->trace, info.trace);

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
    function_type->declaration->identifier.parent_declaration = (void*) declaration;

    assign_generics_to_declaration((void*) declaration, info.generics_collection);
    push(&parser->stack, declaration->body);
    traverse_type(return_type, NULL, &recycle_missing_generics, parser, TraverseGenerics);

    push(&info.declaration_scope->hoisted_declarations, (void*) declaration);
    put(&info.declaration_scope->variables, info.identifier.base, (void*) declaration);

    while(parser->tokenizer->current.type && parser->tokenizer->current.type != ')') {
        Argument argument = { 0 };

        // TODO: remove self keyword
        if(parser->tokenizer->current.type == TokenIdentifier
           && streq(parser->tokenizer->current.trace.source, String("self"))) {
            StructType* const parent_structure = (void*) declaration->identifier.parent_scope;

            if(parent_structure->id == NodeStructType) {
                const Trace trace = next(parser->tokenizer).trace;
                argument.identifier = trace.source;

                Wrapper* wrapper = variable_of((void*) parent_structure->parent, trace, 0);
                apply_type_arguments(wrapper, parser);
                argument.type = wrapper->type;
                unbox((void*) wrapper);
            }
        }

        if(!argument.type) {
            argument.type = (void*) righthand_expression(lefthand_expression(parser), parser, RightDeclaration);
            argument.identifier = expect(parser->tokenizer, TokenIdentifier).trace.source;
        }

        if(!(argument.type->flags & fType)) {
            push(parser->tokenizer->messages,
                 REPORT_ERR(argument.type->trace, strf(0, "'\33[35m%.*s\33[0m' is not a type",
                     PRINT(argument.type->trace.source))));
        }

        Declaration* const argument_declaration = (void*) new_node((Node) {
            .VariableDeclaration = {
                .id = NodeVariableDeclaration,
                .type = argument.type,
                .identifier = {
                    .base = argument.identifier,
                    .parent_scope = (void*) parser->stack.data[0],
                },
            }
        });
        argument_declaration->identifier.parent_declaration = argument_declaration;
        put(&declaration->body->variables, argument.identifier, argument_declaration);

        push(&declaration->arguments, argument);
        push(&function_type->signature, argument.type);

        if(!try(parser->tokenizer, ',', NULL)) break;
    }
    expect(parser->tokenizer, ')');

    if(!declaration->identifier.is_external) {
        expect(parser->tokenizer, '{');
        declaration->body->children = collect_until(parser, &statement, 0, '}');
    }

    close_generics_declaration((void*) declaration);
    pop(&parser->stack);

    return (void*) variable_of((void*) declaration, declaration->trace,
                               fIgnoreStatement | fStatementTerminated * !declaration->identifier.is_external);
}
