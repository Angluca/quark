#include "identifier.h"

#include "../../../parser/type/stringify_type.h"

void compile_identifier(const Identifier identifier, String* line) {
    if(!identifier.is_external) {
        if(identifier.parent_scope && identifier.parent_scope->id == NodeFunctionDeclaration) {
            const Identifier parent_ident = identifier.parent_scope->FunctionDeclaration.identifier;
            compile_identifier(parent_ident, line);
            strf(line, "__");
        }

        if(identifier.parent_scope && identifier.parent_scope->id == NodeStructType
           && !(identifier.parent_declaration->id == NodeVariableDeclaration
                && !(identifier.parent_declaration->type->flags & fType))) {
            const Identifier parent_ident = ((StructType*) (void*) identifier.parent_scope)->parent->identifier;
            compile_identifier(parent_ident, line);
            strf(line, "__");
        }
    }

    // TODO: change `PRINT` macro to `FMT` or `STRFMT`
    strf(line, "%.*s", PRINT(identifier.base));

    if(identifier.parent_declaration->generics.type_arguments_stack.size && !identifier.is_external) {
        stringify_generics(line, last(identifier.parent_declaration->generics.type_arguments_stack),
                           StringifyAlphaNumeric);
    }
}
