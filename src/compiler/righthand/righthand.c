#include "righthand.h"

void comp_BinaryOperation(void* void_self, String* line, Compiler* compiler) {
    BinaryOperation* const self = void_self;

    compile(self->left, line, compiler);

    strf(line,
         self->operator.data[0] == '.' || (self->operator.data[0] == '-' && self->operator.data[1] == '>')
             ? "%.*s"
             : " %.*s ",
         (int) self->operator.size, self->operator.data);

    compile(self->right, line, compiler);
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
