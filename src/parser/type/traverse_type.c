#include "traverse_type.h"

#include "types.h"

static int traverse_generics(Declaration* const declaration, int (*acceptor)(Type*, Type*, void*), void* accumulator,
                             const unsigned flags) {
    if(!(flags & TraverseGenerics) || !declaration->generics.type_arguments_stack.size) return 0;

    const TypeVector generics = last(declaration->generics.type_arguments_stack);
    for(size_t i = 0; i < generics.size; i++) {
        const int result = traverse_type(generics.data[i], NULL, acceptor, accumulator,
                                         flags);
        if(result) return result;
    }

    return 0;
}

static int try_compare_same_declarations(Declaration* type_declaration, const ActionVector type_actions,
                                         Declaration* follower_declaration, const ActionVector follower_actions,
                                         int (*acceptor)(Type*, Type*, void*), void* accumulator,
                                         const unsigned flags) {
    if(type_declaration != follower_declaration) return 0;

    TypeVector type_generics = find_last_generic_action(type_actions, type_declaration);
    TypeVector follower_generics = find_last_generic_action(follower_actions, follower_declaration);

    // TODO: may be incorrect
    if(!type_generics.size && type_declaration->generics.base_type_arguments.size) {
        type_generics = type_declaration->generics.type_arguments_stack.data[0];
    }
    if(!follower_generics.size && follower_declaration->generics.base_type_arguments.size) {
        follower_generics = follower_declaration->generics.type_arguments_stack.data[0];
    }

    for(size_t i = 0; i < type_generics.size; i++) {
        const int result = traverse_type(type_generics.data[i], follower_generics.data[i], acceptor, accumulator,
                                         flags);
        if(result) return result;
    }

    return 0;
}

int traverse_type(Type* type, Type* follower, int (*acceptor)(Type*, Type*, void*), void* accumulator,
                  const unsigned flags) {
    const OpenedType open_follower = open_type(follower, flags & (ActionKeepGlobalState | ActionNoChildCompilation));
    const OpenedType open_type =
            open_type_with_acceptor(type, follower, flags & TraverseIntermediate ? acceptor : 0, accumulator,
                                    flags & (ActionKeepGlobalState | ActionNoChildCompilation));

    int result = 0, result_offset = 0;
    if(!(flags & TraverseIntermediate)) {
        result_offset = !!((result = acceptor(open_type.type, open_follower.type, accumulator)));
    }

    if(result) {
    } else if(follower && open_type.type->id != open_follower.type->id) {
        result = 1;
    } else {
        switch(open_type.type->id) {
            case NodePointerType:
                result = traverse_type(open_type.type->PointerType.base,
                                       open_follower.type ? open_follower.type->PointerType.base : NULL, acceptor,
                                       accumulator, flags);
                break;

            case NodeStructType:
                if(open_follower.type) {
                    result = try_compare_same_declarations((void*) open_type.type->StructType.parent, open_type.actions,
                                                           (void*) open_follower.type->StructType.parent,
                                                           open_follower.actions, acceptor, accumulator, flags);
                    if(result) break;
                }

                result = traverse_generics((void*) open_type.type->StructType.parent, acceptor, accumulator,
                                           flags);
                if(result) break;

                if(open_follower.type && open_follower.type->StructType.fields.size != open_type.type->StructType.fields
                   .size) {
                    result = 1;
                    break;
                }

                for(size_t i = 0; !result && i < open_type.type->StructType.fields.size; i++) {
                    result = traverse_type(open_type.type->StructType.fields.data[i].type,
                                           open_follower.type
                                               ? open_follower.type->StructType.fields.data[i].type
                                               : 0,
                                           acceptor, accumulator, flags);
                }

                break;

            case NodeFunctionType:
                result = traverse_generics((void*) open_type.type->FunctionType.declaration, acceptor, accumulator,
                                           flags);
                break;

            default: ;
        }
    }

    close_type(open_type.actions, flags & (ActionKeepGlobalState | ActionNoChildCompilation));
    close_type(open_follower.actions, flags & (ActionKeepGlobalState | ActionNoChildCompilation));
    return result - result_offset;
}
