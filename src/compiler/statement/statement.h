#ifndef COMPILER_STATEMENT_H
#define COMPILER_STATEMENT_H

#include "../compiler.h"

void comp_StatementWrapper(void* void_self, String* line, Compiler* compiler);

void comp_ReturnStatement(void* void_self, String* line, Compiler* compiler);

void comp_ControlStatement(void* void_self, String* line, Compiler* compiler);

#endif