#include "reference.h"
#include "../type/types.h"

Node* reference(Node* node, const Trace trace) {
    if(node->id == WrapperVariable && node->Wrapper.Variable.is_self_literal) {
        Type* const referenced_self = (void*) reference((void*) node->type, trace);
        node->Wrapper.Variable.declaration->type = referenced_self;
        node->type = referenced_self;

        return node;
    }

    if(node->flags & fType) {
        return (void*) new_type((Type) {
            .PointerType = {
                .id = NodePointerType,
                .flags = fNumeric,
                .trace = trace,
                .base = (void*) node,
            }
        });
    }

    return new_node((Node) {
        .Wrapper = {
            .id = WrapperSurround,
            .trace = trace,
            .type = (void*) reference((void*) node->type, trace),
            .Surround = { node, String("(&"), String(")") },
        }
    });
}

Node* dereference(Node* node, Trace trace, MessageVector* messages) {
    if(node->flags & fType) {
        const OpenedType open = open_type((void*) node, 0);

        if(open.type->id != NodePointerType) {
            push(messages, REPORT_ERR(trace, strf(0, "Cannot de-refence a non-pointer value")));
            close_type(open.actions, 0);
            return node;
        }

        Type* const child = make_type_standalone(open.type->PointerType.base);
        close_type(open.actions, 0);
        return (void*) child;
    }

    return new_node((Node) {
        .Wrapper = {
            .id = WrapperSurround,
            .flags = fMutable,
            .trace = trace,
            .type = (void*) dereference((void*) node->type, trace, messages),
            .Surround = { node, String("(*"), String(")") },
        }
    });
}

