#ifndef COMPILER_H
#define COMPILER_H

#include "../parser/nodes/nodes.h"

typedef struct {
    StringVector lines;
    String indent;
} CompilerSection;

typedef Vector(CompilerSection) CompilerSections;

struct Compiler {
    CompilerSections sections;
    size_t open_section;
    MessageVector* messages;
};

String new_line(Compiler* compiler);

void compile(void* void_node, String* line, Compiler* compiler);

#endif