#include "literals.h"

void comp_NumericLiteral(void* void_self, String* line, Compiler* compiler) {
    (void) compiler;
    NumericLiteral* const self = void_self;

    strf(line, "%ld", self->value);
}

void comp_Missing(void* void_self, String* line, Compiler* compiler) {
    (void) line;
    Missing* const self = void_self;

    push(compiler->messages,
         REPORT_ERR(self->trace, strf(0, "cannot find '\33[35m%.*s\33[0m' in scope", PRINT(self->trace.source))));
}

void comp_External(void* void_self, String* line, Compiler* compiler) {
    (void) compiler;
    External* const self = void_self;

    strf(line, "%.*s", PRINT(self->data));
}
