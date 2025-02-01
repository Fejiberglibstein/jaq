#include "../src/lexer.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define jaq_assert(v, msg, args...)                                                                \
    if (!(v)) {                                                                                    \
        size_t buf_size = snprintf(NULL, 0, msg, args) + 1;                                        \
        char *res = malloc(buf_size);                                                              \
        snprintf(res, buf_size, msg, args);                                                        \
        return res;                                                                                \
    };
char *tok_equal(Token exp, Token actual) {
    jaq_assert(
        actual.type == exp.type, "Types not equal: Expected %d, got %d", exp.type, actual.type
    );
    switch (exp.type) {
    case TOKEN_IDENT:
        jaq_assert(
            strcmp(exp.inner.ident, actual.inner.ident) == 0,
            "inner.ident not equal: Expected %s, got %s",
            exp.inner.ident,
            actual.inner.ident
        );
        break;
    case TOKEN_STRING:
        jaq_assert(
            strcmp(exp.inner.string, actual.inner.string) == 0,
            "inner.string not equal: Expected %s, got %s",
            exp.inner.string,
            actual.inner.string
        );
        break;
    case TOKEN_NUMBER:
        jaq_assert(
            fabs(exp.inner.number - actual.inner.number) < 0.0001,
            "inner.number not equal: Expected %f, got %f",
            exp.inner.number,
            actual.inner.number
        );
        break;
    case TOKEN_KEYWORD:
        jaq_assert(
            exp.inner.keyword == actual.inner.keyword,
            "inner.keyword not equal: Expected %d, got %d",
            exp.inner.keyword,
            actual.inner.keyword
        );
        break;
    default:
        break;
    }

    jaq_assert(
        exp.range.start.col == actual.range.start.col
            && exp.range.start.line == actual.range.start.line
            && exp.range.end.col == actual.range.end.col
            && exp.range.end.line == actual.range.end.line,
        "range not equal: Expected (%d:%d-%d:%d), got (%d:%d-%d:%d)",

        exp.range.start.line,
        exp.range.start.col,
        exp.range.end.line,
        exp.range.end.col,
        actual.range.start.line,
        actual.range.start.col,
        actual.range.end.line,
        actual.range.end.col
    );

    return NULL;
}
#undef jaq_assert

#define LIST(v...) (v), (sizeof(v) / sizeof(*v))
#define EPSILON 0.0001

void lex(char *inp, Token *expected, int len) {
    Lexer l = lex_init(inp);

    for (int i = 0; i < len; i++) {
        LexResult res = lex_next_tok(&l);
        assert(!res.error_message);
        assert(!res.finished);
        Token tok = res.token;
        Token exp = expected[i];

        char *eq = tok_equal(exp, tok);
        if (eq != NULL) {
            printf("%s\n", eq);
            assert(false);
        }
    }
    LexResult res = lex_next_tok(&l);
    assert(res.finished);
}

void test_simple_lex() {
    lex("hii",
        LIST((Token[]) {
			(Token) {
				.range = (Range) {
					.start = (Position) {.col = 1, .line = 1},
					.end = (Position) {.col = 4, .line = 1},
				},
				.type = TOKEN_IDENT,
				.inner.ident = "hii",
			},
		}));
    //     lex("10",
    //         LIST((Token[]) {
    // 			(Token) {
    // 				.range = (Range) {
    // 					.start = (Position) {.col = 0, .line = 0},
    // 					.end = (Position) {.col = 2, .line = 0},
    // 				},
    // 				.type = TOKEN_NUMBER,
    // 				.inner.number = 10,
    // 			},
    // 		}));
}

int main() {
    test_simple_lex();
}
