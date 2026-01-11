#ifndef UNIT_TESTS_C
#define UNIT_TESTS_C

#include <stdio.h>

extern unsigned global_total_tests,
         global_passed_tests,
         global_total_assertions,
         global_passed_assertions;

void test_assert_eq(const char* file, int line, const char* as, size_t a, const char* bs, size_t b);

#define assert_eq(a, b) test_assert_eq(__FILE__, __LINE__, #a, (size_t)(a), #b, (size_t)(b))

void test_complete_test(const char* file, const char* name);

#define test(name) for(int _t = 1; _t; test_complete_test(__FILE__, name), _t = 0)

int test_print_result(const char* file);

#define print_result() test_print_result(__FILE__)

#endif
