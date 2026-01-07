#include "variable.h"

#include "../righthand.h"
#include "../../lefthand/lefthand.h"
#include "../../literal/wrapper.h"
#include "../../statement/statement.h"
#include "../../type/clash_types.h"

Node* parse_variable_declaration(Type* type, IdentifierInfo info, Parser* parser) {
    VariableDeclaration* declaration = (void*) new_node((Node) {
        .VariableDeclaration = {
            .id = NodeVariableDeclaration,
            .trace = stretch(type->trace, info.trace),
            .type = (void*) type,
            .identifier = info.identifier,
        }
    });
    info.identifier.parent_declaration = (void*) declaration;

    push(&info.declaration_scope->hoisted_declarations, (void*) declaration);
    put(&info.declaration_scope->variables, info.identifier.base, (void*) declaration);

    if(type->flags & fConst && try(parser->tokenizer, '=', 0)) {
        declaration->const_value = righthand_expression(lefthand_expression(parser), parser, 14);
        clash_types(declaration->type, declaration->const_value->type, declaration->trace, parser->tokenizer->messages,
                    0);
    }

    return (void*) variable_of((void*) declaration, declaration->trace, fIgnoreStatement);
}

// TODO: remove eval
Node* create_temp_variable(Node* value, Parser* parser, unsigned* set_id) {
    static unsigned id = 0;
    if(set_id) *set_id = id;

    Node* declaration = eval_w(parser->tokenizer->current.trace.filename,
                               strf(NULL, "auto __qv%u = extern<auto> \"\";%c", id++, '\0').data, parser, &statement);
    declaration->StatementWrapper.expression->BinaryOperation.right = value;
    clash_types(declaration->StatementWrapper.expression->BinaryOperation.left->type, value->type, value->trace,
                parser->tokenizer->messages, 0);

    return declaration;
}
