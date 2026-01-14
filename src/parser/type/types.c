#include "types.h"

#include "stringify_type.h"
#include "../compiler/compiler.h"

bool global_in_compiler_step = false;
Compiler* global_compiler_context = NULL;
ActionVector global_actions = { 0 };

Type* new_type(Type type) {
    type.flags |= fType;
    Type* box = (void*) new_node((Node) { .Type = type });
    box->type = box;
    return box;
}

// TODO: possibly move actions to their own file
bool apply_action(const Action action, const unsigned flags) {
    switch(action.type) {
        case ActionNone: return false;

        case ActionApplyGenerics: {
            // for(size_t i = 0; i < action.generics.size; i++) {
            //     if(action.generics.data[i]->id == WrapperAuto
            //        && action.generics.data[i]->Wrapper.Auto.replacement_generic
            //        && action.generics.data[i]->Wrapper.Auto.replacement_generic->generics_declaration
            //        == action.target) {
            //         return false;
            //     }
            // }

            push(&action.target->generics.type_arguments_stack, action.generics);

            if(!(flags & ActionKeepGlobalState)) {
                push(&global_actions, action);
            }

            static bool recursion_stop = false;
            if(global_in_compiler_step && !recursion_stop && !(flags & ActionNoChildCompilation)) {
                recursion_stop = true;

                String unique_key = { 0 };
                stringify_generics(&unique_key, action.generics, StringifyAlphaNumeric);

                recursion_stop = false;

                if(!get(action.target->generics.unique_combinations, unique_key)) {
                    put(&action.target->generics.unique_combinations, unique_key);
                    compile(action.target, NULL, global_compiler_context);
                }
            }

            break;
        }

        case ActionApplyCollection:
            for(size_t i = 0; i < action.collection.size; i++) {
                apply_action(action.collection.data[i], flags);
            }
            break;

        default: unreachable();
    }

    return true;
}

void remove_action(const Action action, const unsigned flags) {
    switch(action.type) {
        case ActionNone: return;

        case ActionApplyGenerics:
            pop(&action.target->generics.type_arguments_stack);

            if(!(flags & ActionKeepGlobalState)) {
                pop(&global_actions);
            }

            break;

        case ActionApplyCollection:
            for(size_t i = action.collection.size; i > 0; i--) {
                remove_action(action.collection.data[i - 1], flags);
            }
            break;

        default: unreachable();
    }
}


Type* peek_type(Type* type, Action* action, const unsigned flags) {
    if(type->id == WrapperAuto || type->id == WrapperVariable || type->id == WrapperSurround) {
        if(apply_action(type->Wrapper.action, flags)) {
            *action = type->Wrapper.action;
        }
    }

    switch(type->id) {
        case WrapperAuto:
            // if(global_in_compiler_step && type->Wrapper.Auto.replacement_generic) {
            //     return peek_type((void*) type->Wrapper.Auto.replacement_generic, action, flags);
            // }

            // if(global_in_compiler_step && type->Wrapper.Auto.test_against) {
            //
            // }

            if(!type->Wrapper.Auto.ref && type->Wrapper.Auto.test_against
               && type->Wrapper.Auto.test_against->id == NodeGenericReference) {
                GenericReference* const reference = &type->Wrapper.Auto.test_against->GenericReference;
                const TypeVectorVector stack = reference->generics_declaration->generics.type_arguments_stack;

                for(size_t i = stack.size - 1; i > 0; i--) {
                    Type* const type_argument = stack.data[i - 1].data[reference->index];

                    if(type_argument->id == WrapperAuto && type_argument->Wrapper.Auto.test_against
                       && type_argument->Wrapper.Auto.test_against->id == NodeGenericReference
                       && type_argument->Wrapper.Auto.test_against->GenericReference.generics_declaration
                       == reference->generics_declaration)
                        continue;

                    return type_argument;
                }

                // return reference->generics_declaration->generics.type_arguments_stack.data[
                //             reference->generics_declaration->generics.type_arguments_stack.size - 2]
                //         .data[reference->index];
            }

            return type->Wrapper.Auto.ref ? type->Wrapper.Auto.ref : type;

        case WrapperVariable:
            return (void*) type->Wrapper.Variable.declaration->const_value;

        case NodeGenericReference:
            return last(type->GenericReference.generics_declaration->generics.type_arguments_stack)
                    .data[type->GenericReference.index];

        default: return type;
    }
}

OpenedType open_type_with_acceptor(Type* type, Type* follower, int (*acceptor)(Type*, Type*, void*),
                                   void* accumulator, const unsigned flags) {
    OpenedType opened_type = { 0 };
    if(!type) return opened_type;
    Action action = { 0 };

    while((opened_type.type = peek_type(type, &action, flags)) != type) {
        type = opened_type.type;
        if(acceptor) acceptor(type, follower, accumulator);

        if(action.type) {
            push(&opened_type.actions, action);
            action.type = 0;
        }
    }

    return opened_type;
}

void close_type(const ActionVector actions, const unsigned flags) {
    for(size_t i = actions.size; i > 0; i--) {
        remove_action(actions.data[i - 1], flags);
    }
    free(actions.data);
}

TypeVector find_last_generic_action(const ActionVector actions, Declaration* const declaration) {
    for(size_t i = actions.size; i > 0; i--) {
        switch(actions.data[i - 1].type) {
            case ActionApplyGenerics:
                if(actions.data[i - 1].target == (void*) declaration) {
                    return actions.data[i - 1].generics;
                }
                break;

            case ActionApplyCollection: {
                const TypeVector found = find_last_generic_action(actions.data[i - 1].collection, declaration);
                if(found.size) return found;
                break;
            }

            default: unreachable();
        }
    }

    return (TypeVector) { 0 };
}

Type* make_type_standalone(Type* type) {
    if(!global_actions.size) return type;

    ActionVector actions = { 0 };
    resv(&actions, global_actions.size);
    memcpy(actions.data, global_actions.data, global_actions.size * sizeof(Action));
    actions.size = global_actions.size;

    return new_type((Type) {
        .Wrapper = {
            .id = WrapperAuto,
            .flags = type->flags,
            .trace = type->trace,
            .action = { ActionApplyCollection, .collection = actions },
            .Auto.ref = type,
        }
    });
}
