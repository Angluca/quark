#include "optional_coalesce.h"

#include "righthand.h"
#include "../statement/scope.h"
#include "declaration/variable.h"

#include "../statement/statement.h"
#include "../type/clash_types.h"
#include "../type/types.h"

static void optional_struct_declaration_compiler(void* self, String* line, Compiler* compiler) {
    (void) self;
    (void) line;
    (void) compiler;
}

Node* parse_optional_coalescing(Node* lefthand, Parser* parser) {
    static VariableDeclaration optional_struct_declaration = {
        .id = NodeVariableDeclaration,
        .identifier = {
            .base = String("Option"),
            .parent_declaration = (void*) &optional_struct_declaration,
            .parent_scope = (void*) &optional_struct_declaration,
        },
        .generics = {
            .base_type_arguments = { .size = 1 },
            .monomorphic_compiler = &optional_struct_declaration_compiler,
        },
    };

    static StructType optional_struct_type = {
        .id = NodeStructType,
        .parent = &optional_struct_declaration,
    };

    next(parser->tokenizer);

    if(lefthand->flags & fType) {
        TypeVector action_generics = { 0 };
        push(&action_generics, (void*) lefthand);

        return (void*) new_type((Type) {
            .Wrapper = {
                .id = WrapperAuto,
                .action = { ActionApplyGenerics, action_generics, (void*) &optional_struct_declaration },
                .Auto.ref = (void*) &optional_struct_type,
            },
        });
    }

    const RighthandOperator operator = global_righthand_operator_table[parser->tokenizer->current.type];
    if(operator.precedence > 1) return NULL;

    const OpenedType lefthand_optional = open_type(lefthand->type, 0);
    close_type(lefthand_optional.actions, 0);
    if(lefthand_optional.type->id != NodeStructType ||
       !streq(lefthand_optional.type->StructType.parent->identifier.base, String("Option"))) {
        return NULL;
    }

    unsigned lefthand_variable_id;
    Node* const lefthand_variable = create_temp_variable(lefthand, parser, &lefthand_variable_id);

    Node* const some_branch_value = righthand_expression(
        new_node((Node) {
            .BinaryOperation = {
                .id = NodeBinaryOperation,
                .left = lefthand_variable,
                .operator = String("."),
                .right = new_node((Node) { .External = { NodeExternal, .data = String("value") } }),
            },
        }), parser, 2);

    Scope* operation_step_collection = (void*) new_node((Node) {
        .Scope = {
            .id = NodeScope,
            .trace = lefthand->trace,
            .wrap_with_brackets = true,
        }
    });
    // push(&operation_step_collection->children, lefthand_variable);

    Node* const some_branch_if_statement = new_node((Node) {
        .ControlStatement = {
            .id = NodeControlStatement,
            .keyword = String("if"),
            .body = new_scope(NULL),
        },
    });
    push(&operation_step_collection->children, some_branch_if_statement);

    Node* const if_cond = new_node((Node) {
        .BinaryOperation = {
            .id = NodeBinaryOperation,
            .left = lefthand_variable,
            .operator = String("."),
            .right = new_node((Node) { .External = { NodeExternal, .data = String("some") } }),
        }
    });
    push(&some_branch_if_statement->ControlStatement.conditions, if_cond);

    const OpenedType opened_optional_value = open_type(some_branch_value->type, 0);
    const bool no_resulting_value = opened_optional_value.type->id == NodeExternal
                                    && streq(opened_optional_value.type->External.data, String("void"));
    close_type(opened_optional_value.actions, 0);

    if(no_resulting_value) {
        push(&some_branch_if_statement->ControlStatement.body->children, some_branch_value);
        return (void*) operation_step_collection;
    }

    static Node false_node = { .External = { NodeExternal, .data = String("false") } };
    static Node true_node = { .External = { NodeExternal, .data = String("true") } };

    TypeVector optional_generics = { 0 };
    push(&optional_generics, some_branch_value->type);

    Node* const operation_result_value = new_node((Node) {
        .StructLiteral = {
            .id = NodeStructLiteral,
            .type = new_type((Type) {
                .Wrapper = {
                    .id = WrapperAuto,
                    .action = { ActionApplyGenerics, optional_generics, (void*) &optional_struct_declaration },
                    .Auto.ref = (void*) &optional_struct_type,
                },
            }),
        },
    });
    push(&operation_result_value->StructLiteral.field_names, String("some"));
    push(&operation_result_value->StructLiteral.field_values, &false_node);

    unsigned operation_result_id;
    Node* const operation_result = create_temp_variable(operation_result_value, parser, &operation_result_id);

    Node* const some_branch_result_optional = new_node((Node) {
        .StructLiteral = {
            .id = NodeStructLiteral,
            .type = new_type((Type) {
                .Wrapper = {
                    .id = WrapperAuto,
                    .action = { ActionApplyGenerics, optional_generics, (void*) &optional_struct_declaration },
                    .Auto.ref = (void*) &optional_struct_type,
                },
            }),
        },
    });
    push(&some_branch_result_optional->StructLiteral.field_names, String("some"));
    push(&some_branch_result_optional->StructLiteral.field_values, &true_node);
    push(&some_branch_result_optional->StructLiteral.field_names, String("value"));
    push(&some_branch_result_optional->StructLiteral.field_values, some_branch_value);

    Node* const some_branch_assignment = new_node((Node) {
        .StatementWrapper = {
            .id = NodeStatementWrapper,
            .expression = new_node((Node) {
                .BinaryOperation = {
                    .id = NodeBinaryOperation,
                    .left = operation_result,
                    .operator = String("="),
                    .right = some_branch_result_optional,
                },
            }),
        },
    });
    push(&some_branch_if_statement->ControlStatement.body->children, some_branch_assignment);

    operation_step_collection->result_value = operation_result;
    operation_step_collection->type = operation_result->type;
    return (void*) operation_step_collection;
}
