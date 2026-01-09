#include <stdlib.h>
#include "unit-tests.c"

#include "tokenizer/tokenizer.c"

int main() {
    test("unit-test test") {
        assert_eq(1, 1);
    }

    print_result();

    test("all tests") {
        assert_eq(test_tokenizer(), 0);
    }

    print_result();
}