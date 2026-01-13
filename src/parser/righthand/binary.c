#include "binary.h"

#include "clargs.h"
#include "righthand.h"
#include "../lefthand/lefthand.h"
#include "../lefthand/reference.h"
#include "../type/clash_types.h"
#include "parser/literal/wrapper.h"
#include "parser/statement/scope.h"

Node* try_binary_postfix(Node* lefthand, Parser* parser) {
    switch(parser->tokenizer->current.type) {
        case '*':
            if(!(lefthand->flags & fType)) break;
            return reference(lefthand, stretch(lefthand->trace, next(parser->tokenizer).trace));

        default: unreachable();
    }

    return NULL;
}

Node* parse_binary_operation(Node* lefthand, const RighthandOperator operator, Parser* parser) {
    const Token operator_token = next(parser->tokenizer);
    Node* righthand = righthand_expression(lefthand_expression(parser), parser, operator.precedence);

    clash_types(lefthand->type, righthand->type, stretch(lefthand->trace, righthand->trace),
                parser->tokenizer->messages, 0);

    Type* type = lefthand->type;
    const Trace trace = stretch(lefthand->trace, righthand->trace);

    switch(operator.type) {
        case RightCompare: {
            static Type boolean_type = {
                .External = {
                    .id = NodeExternal,
                    .type = &boolean_type,
                    .flags = fType | fNumeric,
                    .data = String("bool"),
                },
            };

            type = &boolean_type;
            break;
        }

        case RightRange: {
            static Declaration* range_declaration = NULL;

            if(!range_declaration) {
                range_declaration = find_on_stack_unwrapped(parser->stack, String("Range"));

                if(!range_declaration) {
                    panicf("[fatal] Unable to find declaration for '\33[35mRange\33[0m'");
                }
            }

            StructLiteral* struct_literal = (void*) new_node((Node) {
                .StructLiteral = {
                    .id = NodeStructLiteral,
                    .trace = trace,
                    .type = (void*) variable_of(range_declaration, trace, 0),
                },
            });

            push(&struct_literal->field_names, String("start"));
            push(&struct_literal->field_values, lefthand);
            push(&struct_literal->field_names, String("end"));
            push(&struct_literal->field_values, righthand);

            return (void*) struct_literal;
        }

        default: ;
    }

    return new_node((Node) {
        .BinaryOperation = {
            .id = NodeBinaryOperation,
            .flags = lefthand->flags & righthand->flags,
            .trace = trace,
            .type = type,
            .left = lefthand,
            .operator = operator_token.trace.source,
            .right = righthand,
        }
    });
}
