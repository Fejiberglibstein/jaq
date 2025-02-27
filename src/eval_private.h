#ifndef _EVAL_PRIVATE_H
#define _EVAL_PRIVATE_H

#include "src/eval.h"
#include "src/json.h"
#include "src/json_iter.h"
#include "src/parser.h"

#define _FREE_FREE_LIST(free_list)                                                                 \
    uint __list_size = sizeof(free_list) / sizeof(*free_list);                                     \
    for (uint i = 0; i < __list_size; i++) {                                                       \
        json_free((free_list)[i]);                                                                 \
    }

#define EXPECT_TYPE(j, _type, free_list, ...)                                                      \
    ({                                                                                             \
        Json __j = j;                                                                              \
        if (__j.type != _type) {                                                                   \
            _FREE_FREE_LIST(free_list)                                                             \
            return eval_res_error(TYPE_ERROR(__VA_ARGS__));                                        \
        }                                                                                          \
        __j;                                                                                       \
    })

#define PROPOGATE_INVALID_JSON(j, free_list)                                                       \
    ({                                                                                             \
        EvalResult __r = j;                                                                        \
        PROPOGATE_INVALID(                                                                         \
            __r,                                                                                   \
            free_list,                                                                             \
            EVAL_JSON,                                                                             \
            eval_res_error(TYPE_ERROR("Expected json value, got iterator"))                        \
        );                                                                                         \
        __r.json;                                                                                  \
    })

#define PROPOGATE_INVALID_ITER(j, free_list)                                                       \
    ({                                                                                             \
        EvalResult __r = j;                                                                        \
        PROPOGATE_INVALID(                                                                         \
            __r,                                                                                   \
            free_list,                                                                             \
            EVAL_ITER,                                                                             \
            eval_res_error(TYPE_ERROR("Expected iterator, got json value"))                        \
        );                                                                                         \
        __r.iter;                                                                                  \
    })

#define PROPOGATE_INVALID(__r, free_list, _type, err)                                              \
    if (__r.type != _type) {                                                                       \
        _FREE_FREE_LIST(free_list)                                                                 \
        return __r.type == EVAL_ERR ? __r : err;                                                   \
    }

struct Variable {
    char *name;
    Json value;
};

typedef Vec(struct Variable) VariableStack;

typedef struct {
    /// The input json from invoking the program
    Json input;

    /// The variables that are currently in scope.
    ///
    /// This is represented as a stack so that we can allow for variable
    /// shadowing:
    ///
    ///  [{"foo": 4}].map(|v| v.foo.map(|v| v))
    ///
    /// will create a stack like [v: {"foo": 4}, v: 4].
    VariableStack vars;
} Eval;

EvalResult eval_node(Eval *e, ASTNode *node);
EvalResult eval_function_map(Eval *, ASTNode *);

EvalResult eval_res_json(Json);
EvalResult eval_res_error(char *, ...);
EvalResult eval_res_iter(JsonIterator);

Json vs_get_variable(VariableStack *, char *);

#endif // _EVAL_PRIVATE_H
