#include <clargs.h>
#include <helpers.h>
#include <errno.h>

#include "compiler/compiler.h"
#include "parser/statement/scope.h"
#include "parser/statement/statement.h"
#include "parser/type/types.h"
#include "parser/keywords.h"

typedef Vector(char*) CStringVector;

FunctionDeclaration* entry_declaration() {
    FunctionType* function_type = (void*) new_type((Type) { NodeFunctionType });
    Type* const int_type = new_type((Type) { .External = { NodeExternal, .data = String("int") } });
    push(&function_type->signature, int_type);

    FunctionDeclaration* declaration = (void*) new_node((Node) {
        .FunctionDeclaration = {
            .id = NodeFunctionDeclaration,
            .type = (void*) function_type,
            .identifier = { .base = String("main") },
            .body = new_scope(NULL),
        }
    });
    function_type->declaration = declaration;

    return declaration;
}

int main(int argc, char** argv) {
    char* const name = clname(argc, argv);
    (void) name;

    CStringVector input_files = { 0 };
    char* output_file = "out.c";

    int flag;
    while((flag = clflag()))
        switch(flag) {
            case -1:
                push(&input_files, clarg());
                break;

            case 'o':
                output_file = clarg();
                break;

            case 'l':
                global_library_path = clarg();
                break;

            default: panicf("unknown flag '-%c'\n", flag);
        }

    if(input_files.size == 0) {
        panicf("no input files provided\n");
    }

    char* input_content = fs_readfile(input_files.data[0]);
    if(!input_content) {
        panicf("unable to read file '%s': %s\n", input_files.data[0], strerror(errno));
    }

    // TODO: flag to specify initial size
    init_node_arena(2048);

    MessageVector messages = { 0 };
    Tokenizer tokenizer = new_tokenizer(input_files.data[0], input_content, &messages);
    Parser parser = { &tokenizer };
    Compiler compiler = { .messages = &messages };

    push(&compiler.sections, (CompilerSection) { 0 });
    push(&compiler.sections, (CompilerSection) { 0 });
    push(&compiler.sections.data[0].lines, String("#include <stdint.h>"));
    push(&compiler.sections.data[0].lines, String("#include <stdio.h>"));
    push(&compiler.sections.data[0].lines, String("#include <string.h>"));
    push(&compiler.sections.data[0].lines, String("#include <stdlib.h>"));
    push(&compiler.sections.data[0].lines, String("#include <stdbool.h>"));

    push(&parser.stack, new_scope(NULL));

    FunctionDeclaration* entry = entry_declaration();
    push(&parser.stack, entry->body);

    populate_keyword_table();

    // push(&entry->body->children, eval_w("lib::std", "import lib::std;", &parser, &statement));
    const NodeVector body = collect_until(&parser, &statement, 0, 0);
    resv(&entry->body->children, body.size);
    for(size_t i = 0; i < body.size; i++) {
        push(&entry->body->children, body.data[i]);
    }

    global_in_compiler_step = true;
    String temp_line = { 0 };
    compile(entry, &temp_line, &compiler);

    bool printed_error = false;
    for(size_t i = 0; i < messages.size; i++) {
        if(print_message(messages.data[i])) printed_error = true;
    }

    if(printed_error) return 1;

    FILE* out = fopen(output_file, "w+");
    if(!out) {
        panicf("unable to output file '%s' to write: %s\n", output_file, strerror(errno));
    }

    for(size_t i = 0; i < compiler.sections.size; i++) {
        if(i) fprintf(out, "\n");
        for(size_t j = 0; j < compiler.sections.data[i].lines.size; j++) {
            fprintf(out, "%.*s\n", PRINT(compiler.sections.data[i].lines.data[j]));
        }
    }
}
