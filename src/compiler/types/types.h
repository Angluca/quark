#ifndef COMPILER_TYPES_H
#define COMPILER_TYPES_H

#include "../compiler.h"

void comp_PointerType(void* void_self, String* line, Compiler* compiler);

void comp_FunctionType(void* void_self, String* line, Compiler* compiler);

void comp_GenericReference(void* void_self, String* line, Compiler* compiler);

void comp_StructType(void* void_self, String* line, Compiler* compiler);

#endif