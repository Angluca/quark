#ifndef COMPILER_WRAPPER_H
#define COMPILER_WRAPPER_H

#include "../compiler.h"

void comp_Variable(void* void_self, String* line, Compiler* compiler);

void comp_Auto(void* void_self, String* line, Compiler* compiler);

void comp_Surround(void* void_self, String* line, Compiler* compiler);

#endif