#include "../../src/tokenizer/trace.h"
#include "../unit-tests.c"

int test_tokenizer() {
    MessageVector messages = { 0 };

    test("basic tokens") {
        Tokenizer tokenizer = new_tokenizer("TEST TOKENIZER", "a b c 1 2 3 def 456", &messages);

        static char types[] = {
            TokenIdentifier, TokenIdentifier, TokenIdentifier,
            TokenNumber, TokenNumber, TokenNumber,
            TokenIdentifier, TokenNumber,
        };
        static char* strings[] = { "a", "b", "c", "1", "2", "3", "def", "456" };

        for(int i = 0; i < sizeof(types) / sizeof(*types); i++) {
            const Token token = next(&tokenizer);
            assert_eq(token.type, types[i]);
            assert_eq(streq(token.trace.source, (String) { strlen(strings[i]), 0, strings[i] }), true);
        }
    }

    return print_result();
}