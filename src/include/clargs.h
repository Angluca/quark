#ifndef CLARGS_H
#define CLARGS_H

#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"

#define panicf(fmt...) (fprintf(stderr, "\33[31;1merror:\33[0m " fmt), exit(EXIT_FAILURE))

extern int global_argc;
extern char** global_argv;

char* clname(int argc, char** argv);

int clflag();

char* clarg();

#endif
