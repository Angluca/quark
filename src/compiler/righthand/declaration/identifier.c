#include "identifier.h"

#include "../../../parser/type/stringify_type.h"

StringHashSet global_c_keywords = 0;

void populate_global_c_keywords() {
    // https://en.cppreference.com/w/c/keyword.html
    char* keywords[] = {
        "alignas", "alignas", "alignof", "auto", "bool", "break", "case", "char", "const", "constexpr", "continue",
        "default", "do", "double", "else", "enum", "extern", "false", "float", "for", "goto", "if", "inline", "int",
        "long", "nullptr", "register", "restrict", "return", "short", "signed", "sizeof", "static", "static_assert",
        "struct", "switch", "thread_local", "true", "typedef", "typeof", "typeof_unequal", "union", "unsigned", "void",
        "volatile", "while", "_Alignas", "_Alignof", "_Atomic", "_BitInt", "_Bool", "_Complex", "_Decimal128",
        "_Decimal32", "_Decimal64", "_Generic", "_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local",
    };

    for(int i = 0; i < sizeof(keywords) / sizeof(char*); i++) {
        put(&global_c_keywords, ((String) { strlen(keywords[i]), 0, keywords[i] }));
    }
}

DeclarationHashMap global_function_identifiers = 0;

void compile_identifier_base(const String base, String* line) {
    strf(line, "%.*s", PRINT(base));

    if(get(global_c_keywords, base)) {
        strf(line, "_");
    }
}

void compile_identifier(const Identifier identifier, String* line) {
    String result = { 0 };

    if(!identifier.is_external) {
        if(identifier.parent_scope && identifier.parent_scope->id == NodeFunctionDeclaration) {
            const Identifier parent_ident = identifier.parent_scope->FunctionDeclaration.identifier;
            compile_identifier(parent_ident, &result);
            strf(&result, "__");
        }

        if(identifier.parent_scope && identifier.parent_scope->id == NodeStructType
           && !(identifier.parent_declaration->id == NodeVariableDeclaration
                && !(identifier.parent_declaration->type->flags & fType))) {
            const Identifier parent_ident = ((StructType*) (void*) identifier.parent_scope)->parent->identifier;
            compile_identifier(parent_ident, &result);
            strf(&result, "__");
        }
    }

    // TODO: change `PRINT` macro to `FMT` or `STRFMT`
    compile_identifier_base(identifier.base, &result);

    if(identifier.parent_declaration->generics.type_arguments_stack.size && !identifier.is_external) {
        stringify_generics(&result, last(identifier.parent_declaration->generics.type_arguments_stack),
                           StringifyAlphaNumeric);
    }

    if(identifier.function_declaration_counter) strf(&result, "%u", identifier.function_declaration_counter);

    if(!identifier.function_declaration_counter && identifier.parent_declaration->id == NodeFunctionDeclaration) {
        const size_t initial_size = result.size;
        unsigned counter = 0;
        Declaration** existing_declaration;

        while(((existing_declaration = get(global_function_identifiers, result)))
              && *existing_declaration != identifier.parent_declaration) {
            result.size = initial_size;
            strf(&result, "%u", ++counter);
        }

        if(counter) {
            identifier.parent_declaration->identifier.function_declaration_counter = counter;
        }

        if(!existing_declaration) {
            put(&global_function_identifiers, result, identifier.parent_declaration);
        }
    }

    strf(line, "%.*s", PRINT(result));
}
