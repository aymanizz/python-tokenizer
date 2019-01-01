#include <stddef.h>
#include <string.h>

#include "lib/munit/munit.h"

#include "src/token.c"
#include "src/scanner.c"

static MunitResult
test_name(const MunitParameter params[], void* data) {
    Scanner scanner;
    const char *tests_ok[][2] = {
        {"_14_nameCASE__", "_14_nameCASE__"},
        {"Hell_o_14", "Hell_o_14"},
        {"name-name", "name"},
        {"a12$12", "a12"},
        {"name other tokens", "name"}
    };
    int ok_cases = sizeof(tests_ok) / sizeof(*tests_ok);

    for (int i = 0; i < ok_cases; ++i) {
        initScanner(&scanner, tests_ok[i][0]);
        Token matched = name(&scanner);
        Token expected = (Token) {
            .type = TOKEN_NAME, .start = tests_ok[i][1],
            .length = strlen(tests_ok[i][1]),
            .line = 1, .column = 0
        };

        munit_assert(matched.type == expected.type);
        munit_assert_int(matched.length, ==, expected.length);
        munit_assert_memory_equal(matched.length, matched.start, expected.start);
        munit_assert_int(matched.line, ==, expected.line);
        munit_assert_int(matched.column, ==, expected.column);
    }

    return MUNIT_OK;
}

static MunitResult
test_string(const MunitParameter params[], void* data) {
    Scanner scanner;
    const char *tests_ok[][2] = {
        {"\"string literal\"", "\"string literal\""},
        {"'string literal'", "'string literal'"},
        {"\"string\" other tokens", "\"string\""},
        {"'c'other tokens", "'c'"},
        {"'new\\\nline\\\nskipped'", "'new\\\nline\\\nskipped'"},
        {"'''multi\nline\nstrings'''", "'''multi\nline\nstrings'''"}
    };
    const char *tests_fail[] = {
        "\"unterminated",
        "'unterminated",
        "'''unterminated\nmultiline",
        "\"terminated on\n newline\"",
        "'terminated on\n newline'",
    };
    int ok_cases = sizeof(tests_ok) / sizeof(*tests_ok);
    int fail_cases = sizeof(tests_fail) / sizeof(*tests_fail);

    for (int i = 0; i < ok_cases; ++i) {
        initScanner(&scanner, tests_ok[i][0]);
        Token matched = string(&scanner);
        Token expected = (Token){
            .type = TOKEN_STRING, .start = tests_ok[i][1],
            .length = strlen(tests_ok[i][1]),
            .line = 1, .column = 0
        };

        munit_assert(matched.type == expected.type);
        munit_assert_int(matched.length, ==, expected.length);
        munit_assert_memory_equal(matched.length, matched.start, expected.start);
        munit_assert_int(matched.line, ==, expected.line);
        munit_assert_int(matched.column, ==, expected.column);
    }

    for (int i = 0; i < fail_cases; ++i) {
        initScanner(&scanner, tests_fail[i]);
        Token matched = string(&scanner);
        munit_assert(matched.type == TOKEN_ERROR);
    }

    return MUNIT_OK;
}

static MunitResult
test_number(const MunitParameter params[], void *data) {
    Scanner scanner;
    const char *tests_ok[][2] = {
        {"1", "1"},
        {"6.7", "6.7"},
        {".25", ".25"},
        {"84.", "84."},
        {".25.25", ".25"},
        {"25.12.67", "25.12"},
        {"21street", "21"}
    };
    int ok_cases = sizeof(tests_ok) / sizeof(*tests_ok);

    for (int i = 0; i < ok_cases; ++i) {
        initScanner(&scanner, tests_ok[i][0]);
        Token matched = number(&scanner);
        Token expected = (Token){
            .type = TOKEN_NUMBER, .start = tests_ok[i][1],
            .length = strlen(tests_ok[i][1]),
            .line = 1, .column = 0
        };

        munit_assert_int(matched.type, ==, expected.type);
        munit_assert_int(matched.length, ==, expected.length);
        munit_assert_memory_equal(matched.length, matched.start, expected.start);
        munit_assert_int(matched.line, ==, expected.line);
        munit_assert_int(matched.column, ==, expected.column);
    }

    return MUNIT_OK;
}

static MunitResult
test_level(const MunitParameter params[], void *data) {
    Scanner scanner;
    const char * const tests_lvl[][4] = {
        // level is -1
        {"())", "{}}", "(}]", "[]}"},
        // level is 0
        {"()", "{}", "[]", "[(({[{}]}))]"},
        // level is 1
        {"(()", "{)(", "[}[", "([)"}
    };

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            const char * const test_case = tests_lvl[i][j];
            initScanner(&scanner, test_case);
            Token token;

            for (size_t i = 0; i < strlen(test_case); ++i) {
                token = scanToken(&scanner);
            }
            
            fprintf(stderr, "\n=== test case '%s' ===\n", test_case);

            munit_assert_int(scanner.level, ==, i - 1);

            token = scanToken(&scanner);
            if (i - 1 == 0) {
                munit_assert_int(token.type, ==, TOKEN_ENDMARKER);
            } else {
                munit_assert_int(token.type, ==, TOKEN_ERROR);
            }
        }
    }

    return MUNIT_OK;
}

static MunitTest test_suite_tests[] = {
    {"name test", test_name, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"string test", test_string, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"number test", test_number, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"level test", test_level, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

static const MunitSuite test_suite = {
    "", test_suite_tests, NULL, 1, MUNIT_TEST_OPTION_NONE
};

int main(int argc, char *argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
    return munit_suite_main(&test_suite, "python-tokenizer", argc, argv);
}