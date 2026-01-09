#ifndef UNIT_TESTS_C
#define UNIT_TESTS_C

#include "../src/compiler/compiler.h"
#include <stdio.h>

unsigned global_total_tests = 0,
         global_passed_tests = 0,
         global_total_assertions = 0,
         global_passed_assertions = 0;

void _assert_eq(const char* file, const int line, const char* as, const size_t a, const char* bs, const size_t b) {
    global_total_assertions++;
    global_passed_assertions += a == b;

    if(a != b) {
        fprintf(stderr, "%s:%d \33[1massertion failed:\33[0m %s (%zu) != %s (%zu)\n",
                file, line, as, a, bs, b);
    }
}

#define assert_eq(a, b) _assert_eq(__FILE__, __LINE__, #a, (size_t)(a), #b, (size_t)(b))

void _complete_test(const char* file, const char* name) {
    global_total_tests++;
    global_passed_tests += global_passed_assertions == global_total_assertions;

    fprintf(global_passed_assertions == global_total_assertions
                ? stdout
                : stderr,
            global_passed_assertions == global_total_assertions
                ? "\33[42;1m PASSED \33[0m %-32s \33[1m%-32s\33[0m [%u/%u assertions]\n"
                : "\33[41;1m FAILED \33[0m %-32s \33[1m%-32s\33[0m [%u/%u assertions]\n",
            file, name, global_passed_assertions, global_total_assertions);

    global_passed_assertions = global_total_assertions = 0;
}

#define test(name) \
    for(int _t = 1; _t; _complete_test(__FILE__, name), _t = 0)

int _print_result(const char* file) {
    fprintf(global_total_tests == global_passed_tests ? stdout : stderr, "\n%-32s %u / %u test(s) passed\n\n",
            file, global_passed_tests, global_total_tests);
    const int result = global_passed_tests != global_total_tests;
    global_passed_tests = global_total_tests = 0;
    return result;
}

#define print_result() _print_result(__FILE__)

#endif
