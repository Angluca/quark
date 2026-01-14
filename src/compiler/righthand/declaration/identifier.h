#ifndef COMPILER_IDENTIFIER_H
#define COMPILER_IDENTIFIER_H

#include "../../compiler.h"


typedef HashMap(UsableVoid) StringHashSet;

extern StringHashSet global_c_keywords;

void compile_identifier_base(String base, String* line);

void compile_identifier(Identifier identifier, String* line);

void populate_global_c_keywords();

#endif