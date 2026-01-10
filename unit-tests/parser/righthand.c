#include <parser/righthand/righthand.h>
#include <parser/statement/scope.h>
#include <parser/statement/statement.h>
#include "../unit-tests.h"

int test_righthand() {
    MessageVector messages = { 0 };
    Parser parser = { 0 };
    push(&parser.stack, new_scope(NULL));

    test("binary operation & order of operations") {
        Tokenizer tokenizer = new_tokenizer("TEST RIGHTHAND", "1 + 2 * 3", &messages);
        parser.tokenizer = &tokenizer;

        BinaryOperation* const expr = (void*) expression(&parser);
        assert_eq(expr->id, NodeBinaryOperation);
        assert_eq(streq(expr->operator, String("+")), true);

        assert_eq(expr->right->id, NodeBinaryOperation);
        assert_eq(streq(expr->right->BinaryOperation.operator, String("*")), true);

        unbox((void*) expr);

        assert_eq(tokenizer.current.type, 0);
        assert_eq(messages.size, 0);
    }

    test("binary '*' vs ternary '*'") {
        Tokenizer tokenizer = new_tokenizer("TEST RIGHTHAND", "int * x; 4 * 5", &messages);
        parser.tokenizer = &tokenizer;

        Node* ignored = (void*) statement(&parser);
        assert_eq(ignored->id, NodeNone);
        Wrapper* variable = (void*) ignored->type;
        assert_eq(variable->id, WrapperVariable);

        BinaryOperation* bin = (void*) expression(&parser);
        assert_eq(bin->id, NodeBinaryOperation);

        assert_eq(tokenizer.current.type, 0);
        assert_eq(messages.size, 0);
    }

    test("declarations, declarations as variables, and binary/externals type matching") {
        Tokenizer tokenizer = new_tokenizer("TEST RIGHTHAND", "extern a a; extern b b + a", &messages);
        parser.tokenizer = &tokenizer;

        unbox((void*) statement(&parser));

        BinaryOperation* bin = (void*) expression(&parser);
        assert_eq(bin->id, NodeBinaryOperation);
        assert_eq(bin->left->id, WrapperVariable);
        assert_eq(bin->right->id, WrapperVariable);

        assert_eq(messages.size, 1);
        pop(&messages);

        assert_eq(tokenizer.current.type, 0);
    }

    test("assignment operator") {
        Tokenizer tokenizer = new_tokenizer("TEST RIGHTHAND", "int y = 5; y = 4; 4 = 4", &messages);
        parser.tokenizer = &tokenizer;

        StatementWrapper* decl_assignment = (void*) statement(&parser);
        assert_eq(decl_assignment->id, NodeStatementWrapper);
        assert_eq(decl_assignment->expression->id, NodeBinaryOperation);
        assert_eq(streq(decl_assignment->expression->BinaryOperation.operator, String("=")), true);

        StatementWrapper* assignment = (void*) statement(&parser);
        assert_eq(assignment->id, NodeStatementWrapper);
        assert_eq(assignment->expression->id, NodeBinaryOperation);
        assert_eq(streq(assignment->expression->BinaryOperation.operator, String("=")), true);
        assert_eq(messages.size, 0);

        BinaryOperation* failed_assignment = (void*) expression(&parser);
        assert_eq(failed_assignment->id, NodeBinaryOperation);
        assert_eq(streq(failed_assignment->operator, String("=")), true);
        assert_eq(messages.size, 1);
        pop(&messages);

        assert_eq(tokenizer.current.type, 0);
    }

    // test("function calls & external functions") {
    //     Tokenizer tokenizer = new_tokenizer("TEST RIGHTHAND", )
    // }

    return print_result();
}
