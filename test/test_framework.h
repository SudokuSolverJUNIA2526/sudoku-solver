//
// Created by Joan DELAYAT on 03/06/2026.
//
//Pour test unitaire
#ifndef SUDOKU_SOLVER_TEST_FRAMEWORK_H
#define SUDOKU_SOLVER_TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>

static int _tests_run    = 0;
static int _tests_passed = 0;
static int _tests_failed = 0;

#define ASSERT(condition, msg)                                          \
do {                                                                \
_tests_run++;                                                   \
if (condition) {                                                \
_tests_passed++;                                            \
} else {                                                        \
_tests_failed++;                                            \
fprintf(stderr, "  [FAIL] %s:%d — %s\n",                  \
__FILE__, __LINE__, msg);                           \
}                                                               \
} while (0)

#define ASSERT_EQ(a, b, msg)   ASSERT((a) == (b), msg)
#define ASSERT_NEQ(a, b, msg)  ASSERT((a) != (b), msg)
#define ASSERT_NULL(p, msg)    ASSERT((p) == NULL, msg)
#define ASSERT_NOTNULL(p, msg) ASSERT((p) != NULL, msg)

#define RUN_TEST(fn)                        \
do {                                    \
printf("[TEST] %s\n", #fn);         \
fn();                               \
} while (0)

#define PRINT_RESULTS()                                                      \
do {                                                                     \
printf("\n=== Results: %d/%d passed", _tests_passed, _tests_run);   \
if (_tests_failed)                                                   \
printf(", %d FAILED", _tests_failed);                           \
printf(" ===\n");                                                    \
} while (0)

#define EXIT_RESULTS() exit(_tests_failed ? 1 : 0)


#endif //SUDOKU_SOLVER_TEST_FRAMEWORK_H