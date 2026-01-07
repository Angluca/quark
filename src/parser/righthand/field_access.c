#include "field_access.h"

#include "../lefthand/reference.h"
#include "../type/types.h"
#include "../statement/scope.h"
#include "declaration/declaration.h"

Node* parse_field_access(Node* lefthand, Parser* parser) {
    Type* type = lefthand->type;
    if(parser->tokenizer->current.type == TokenRightArrow) {
        type = (void*) dereference((void*) type, lefthand->trace, parser->tokenizer->messages);
    }

    const OpenedType opened = open_type(type, 0);
    StructType* const struct_type = (void*) opened.type;

    String operator_token = next(parser->tokenizer).trace.source;
    Token field_token = expect(parser->tokenizer, TokenIdentifier);

    if(struct_type->id != NodeStructType) {
        push(parser->tokenizer->messages, REPORT_ERR(lefthand->trace,
                 strf(0, "'\33[35m%.*s\33[0m' is not a structure", PRINT(lefthand->trace.source))));
        close_type(opened.actions, 0);
        return lefthand;
    }

    ssize_t found_index = -1;
    for(ssize_t i = 0; i < struct_type->fields.size; i++) {
        if(streq(field_token.trace.source, struct_type->fields.data[i].identifier)) {
            found_index = i;
        }
    }

    if(found_index < 0) {
        Wrapper* child = find_in_scope(*struct_type->static_body, field_token.trace);

        if(child) {
            lefthand->type = make_type_standalone(lefthand->type);
            child->Variable.bound_self_argument = lefthand;

            child->type = make_type_standalone(child->type);

            close_type(opened.actions, 0);
            return child->type->id == WrapperAuto
                       ? new_node((Node) {
                           .Wrapper = {
                               .id = WrapperSurround,
                               .action = child->type->Wrapper.action,
                               .Surround.child = (void*) child,
                           },
                       })
                       : (void*) child;
        }

        push(parser->tokenizer->messages,
             REPORT_ERR(field_token.trace, strf(0, "no field named '\33[35m%.*s\33[0m' on struct '\33[35m%.*s\33[0m'",
                 PRINT(field_token.trace.source), PRINT(lefthand->trace.source))));
        push(parser->tokenizer->messages, see_declaration((void*) struct_type, lefthand->trace));
        return lefthand;
    }

    Type* field_type = make_type_standalone(struct_type->fields.data[found_index].type);
    close_type(opened.actions, 0);

    return new_node((Node) {
        .BinaryOperation = {
            .id = NodeBinaryOperation,
            .flags = fMutable | (lefthand->flags & fConstExpr),
            .trace = stretch(lefthand->trace, field_token.trace),
            .type = field_type,
            .left = lefthand,
            .operator = operator_token,
            .right = new_node((Node) {
                .External = { NodeExternal, .data = field_token.trace.source },
            }),
        }
    });
}
