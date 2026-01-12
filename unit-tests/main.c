#include <stdlib.h>
#include <parser/keywords.h>
#include <parser/nodes/nodes.h>
#include "unit-tests.h"

unsigned global_total_tests = 0,
         global_passed_tests = 0,
         global_total_assertions = 0,
         global_passed_assertions = 0;

void test_assert_eq(const char* file, const int line, const char* as, const size_t a, const char* bs, const size_t b) {
    global_total_assertions++;
    global_passed_assertions += a == b;

    if(a != b) {
        printf("%s:%d \33[31;1massertion failed:\33[0m %s (%zu) != %s (%zu)\n",
               file, line, as, a, bs, b);
    }
}

void test_complete_test(const char* file, const char* name) {
    global_total_tests++;
    global_passed_tests += global_passed_assertions == global_total_assertions;

    printf(global_passed_assertions == global_total_assertions
               ? "\33[42;1m PASSED \33[0m %s \33[1m%s\33[0m [%u/%u assertions]\n"
               : "\33[41;1m FAILED \33[0m %s \33[1m%s\33[0m [%u/%u assertions]\n",
           file, name, global_passed_assertions, global_total_assertions);

    global_passed_assertions = global_total_assertions = 0;
}

int test_print_result(const char* file) {
    printf("\n\33[3%d;1m%s %u / %u test(s) passed\33[0m\n\n",
           (global_passed_tests == global_total_tests) + 1,
           file, global_passed_tests, global_total_tests);
    const int result = global_passed_tests != global_total_tests;
    global_passed_tests = global_total_tests = 0;
    return result;
}

int test_tokenizer();

int test_literal();

int test_lefthand();

int test_righthand();

int main() {
    init_node_arena(2048);
    populate_keyword_table();

    test("unit-test assertion") {
        assert_eq(1, 1);
        assert_eq(0, 0);
    }

    print_result();

    test("all tests") {
        const int tally
                = test_tokenizer()
                  + test_literal()
                  + test_lefthand()
                  + test_righthand();

        assert_eq(tally, 0);
    }

    print_result();
}
