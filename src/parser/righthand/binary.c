#include "binary.h"

#include "righthand.h"
#include "../lefthand/lefthand.h"
#include "../lefthand/reference.h"
#include "../type/clash_types.h"

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

    return new_node((Node) {
        .BinaryOperation = {
            .id = NodeBinaryOperation,
            .flags = lefthand->flags & righthand->flags,
            .trace = stretch(lefthand->trace, righthand->trace),
            .type = lefthand->type,
            .left = lefthand,
            .operator = operator_token.trace.source,
            .right = righthand,
        }
    });
}
