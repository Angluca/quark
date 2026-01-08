#include "righthand.h"

void comp_BinaryOperation(void* void_self, String* line, Compiler* compiler) {
    BinaryOperation* const self = void_self;

    strf(line, "(");
    compile(self->left, line, compiler);
    strf(line, " %.*s ", (int) self->operator.size, self->operator.data);
    compile(self->left, line, compiler);
    strf(line, ")");
}

void comp_FunctionCall(void* void_self, String* line, Compiler* compiler) {
    FunctionCall* const self = void_self;

    compile(self->function, line, compiler);
    strf(line, "(");
    for(size_t i = 0; i < self->arguments.size; i++) {
        if(i) strf(line, ", ");
        compile(self->arguments.data[i], line, compiler);
    }
    strf(line, ")");
}

