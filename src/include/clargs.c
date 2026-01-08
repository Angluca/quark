#include "clargs.h"

int global_argc;
char** global_argv;

char* clname(const int argc, char** argv) {
    global_argc = argc - 1;
    global_argv = argv + 1;
    return *argv;
}

int clflag() {
    if(!global_argc) return 0;
    if(**global_argv == '-') {
        global_argc--;
        return (*global_argv++)[1];
    }
    return -1;
}

char* clarg() {
    if(!global_argc--) {
        panicf("expected an argument\n");
    }
    return *global_argv++;
}
