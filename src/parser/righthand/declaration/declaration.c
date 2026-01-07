#include "declaration.h"

#include "function.h"
#include "identifier.h"
#include "variable.h"

Node* parse_declaration(Node* type, Token identifier, Parser* parser) {
    if(!(type->flags & fType)) {
        push(parser->tokenizer->messages,
             REPORT_ERR(type->trace, String("expected a type before declaration identifier")));
        push(parser->tokenizer->messages, REPORT_HINT(String("also try '\33[35mtypeof(expr)\33[0m'")));
        type = (void*) type->type;
    }

    IdentifierInfo info = new_identifier(identifier, parser, IdentifierDeclaration);

    if(try(parser->tokenizer, '(', 0)) {
        return parse_function_declaration((void*) type, info, parser);
    }

    return parse_variable_declaration((void*) type, info, parser);
}
