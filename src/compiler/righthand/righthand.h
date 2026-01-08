#ifndef COMPILER_RIGHTHAND_H
#define COMPILER_RIGHTHAND_H

#include "../compiler.h"

void comp_BinaryOperation(void* void_self, String* line, Compiler* compiler);

void comp_FunctionCall(void* void_self, String* line, Compiler* compiler);

#endif