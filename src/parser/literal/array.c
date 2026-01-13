#include "array.h"

#include "clargs.h"
#include "wrapper.h"
#include "../statement/statement.h"
#include "../righthand/righthand.h"
#include "../type/clash_types.h"
#include "../type/types.h"
#include "parser/statement/scope.h"

Declaration* fetch_slice_declaration(Parser* parser) {
    static Declaration* declaration = NULL;

    if(!declaration) {
        declaration = find_on_stack_unwrapped(parser->stack, String("Slice"));

        if(!declaration) {
            panicf("[fatal] failed to find declaration for '\33[35mSlice<T>\33[0m'");
        }
    }

    return declaration;
}

Node* parse_array_literal(const Trace trace_start, Parser* parser) {
    const NodeVector field_values = collect_until(parser, &expression, ',', ']');

    if(field_values.data && field_values.data[0]->flags & fType) {
        Wrapper* slice = variable_of(fetch_slice_declaration(parser), field_values.data[0]->trace, 0);

        TypeVector generics = { 0 };
        push(&generics, (void*) field_values.data[0]);
        slice->action = (Action) { ActionApplyGenerics, generics, slice->Variable.declaration };

        return (void*) slice;
    }

    StringVector field_names = { 0 };
    resv(&field_names, field_values.size);
    memset(field_names.data, 0, field_values.size * sizeof(String));

    Type* array_type = new_type((Type) {
        .Wrapper = {
            .id = WrapperAuto,
            .trace = trace_start,
        },
    });

    for(size_t i = 0; i < field_values.size; i++) {
        clash_types(array_type, field_values.data[i]->type, field_values.data[i]->trace, parser->tokenizer->messages,
                    0);
    }

    static StringVector empty_field_names = { 0 };
    if(!empty_field_names.size) {
        resv(&empty_field_names, 2);
        memset(empty_field_names.data, 0, sizeof(String[2]));
    }

    Node* slice = eval("array", "Slice {}", parser);
    slice->StructLiteral.type->Wrapper.action.generics.data[0] = array_type;
    slice->StructLiteral.field_names = empty_field_names;

    Node* data_literal = new_node((Node) {
        .StructLiteral = {
            .id = NodeStructLiteral,
            .type = new_type((Type) {
                .Wrapper = {
                    .id = WrapperSurround,
                    .Surround = { (void*) array_type, {}, String("[]") },
                },
            }),
            .field_names = field_names,
            .field_values = field_values,
        }
    });
    push(&slice->StructLiteral.field_values, data_literal);

    Node* array_size = new_node((Node) {
            .NumericLiteral = {
                .id = NodeNumericLiteral,
                .value = (int64_t) field_values.size,
            },
        }
    );
    push(&slice->StructLiteral.field_values, array_size);

    return slice;
}
