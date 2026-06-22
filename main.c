#include <stdio.h>
#include <string.h>

#include "../sudoku-solver/entity/grid-internal.h"
#include "../sudoku-solver/entity/sudoku_types.h"
#include "../sudoku-solver/repository/io.h"
#include "../sudoku-solver/entity/grid.h"
#include "console/cli.h"
#include "entity/generator.h"
#include "entity/subset.h"
#include "service/service_solver.h"

static int run    = 0;
static int passed = 0;
static int failed = 0;

#define ASSERT(condition, msg)                                              \
    do {                                                               \
        run++;                                                       \
        if (condition) {                                                    \
            passed++;                                                \
            printf("  [PASS] %s\n", (msg));                           \
        } else {                                                       \
            failed++;                                                \
            printf("  [FAIL] %s  (ligne %d)\n", (msg), __LINE__);     \
        }                                                              \
    } while (0)

static void testInitGrid(void)
{
    printf("\n[TEST] initGrid\n");

    initGrid();

    for (int k = 0; k < 81; k++) {
        ASSERT(grid[k].value == 0, "value = 0 after init");
    }
    for (int k = 0; k < 81; k++) {
        for (int d = 0; d < 9; d++) {
            ASSERT(grid[k].possible[d] != 0, "every values possibles");
        }
    }
    ASSERT(history_index == 0, "empty history after init");
}

static void testSetTileValue(void)
{
    SudokuTiles *t;

    printf("\n[TEST] setTileValue\n");

    initGrid();
    t = &grid[0]; /* case (1,1) */

    setTileValue(t, 5, 0);

    ASSERT(t->value == 5, "fixed value: 5");
    for (int d = 0; d < 9; d++) {
        if (d == 4) {
            ASSERT(t->possible[d] != 0, "only value 5 possible");
        } else {
            ASSERT(t->possible[d] == 0, "other(s) values possible");
        }
    }
    ASSERT(history_index == 1,        "history : 1 input");
    ASSERT(history[0].tile == t,      "history : Great i guess");
    ASSERT(history[0].value == 5,     "historiy : value 5");
    ASSERT(history[0].supposed == 0,  "historiy : supposed=0");

    // Test supposed=1
    initGrid();
    t = &grid[40]; /* case (5,5) */
    setTileValue(t, 7, 1);
    ASSERT(history[0].supposed == 1, "history : supposed=1");
    ASSERT(t->value == 7,              "fixed value: 7");
}

static void testDisp(void)
{
    printf("\n[TEST] dispFinal\n");
    initGrid();
    setTileValue(&grid[0],  7, 0); /* (1,1) */
    setTileValue(&grid[40], 5, 0); /* (5,5) */
    setTileValue(&grid[80], 3, 0); /* (9,9) */

    dispFinal();
    ASSERT(1, "dispFinal no shut");

    printf("\n[TEST] dispPossible\n");
    dispPossible();
    ASSERT(1, "dispPossible no shut");
}

static void testIoFile(void)
{
    const char *tmp = "sudoku_test.txt";
    char        res;

    printf("\n[TEST] loadGridFromFile / saveGridToFile\n");

    // Construire une grille, la sauvegarder, la recharger
    initGrid();
    setTileValue(&grid[0],  7, 0);
    setTileValue(&grid[5],  3, 0);
    setTileValue(&grid[9],  6, 0);
    setTileValue(&grid[11], 2, 0);

    res = saveGridToFile(tmp);
    ASSERT(res == 1, "saveGridToFile return ok");

    initGrid();
    res = loadGridFromFile(tmp);
    ASSERT(res == 1,                      "loadGridFromFile return ok");
    ASSERT(grid[0].value  == 7,         "case (1,1) = 7 after loading");
    ASSERT(grid[5].value  == 3,         "case (1,6) = 3 after loading");
    ASSERT(grid[9].value  == 6,         "case (2,1) = 6 after loading");
    ASSERT(grid[1].value  == 0,         "case (1,2) = 0 (unknown)");

    // Test erreur fichier non valide
    res = loadGridFromFile("/joan/delayat.txt");
    ASSERT(res == 0, "loadGridFromFile return ko if file not existing");

    res = loadGridFromFile(NULL);
    ASSERT(res == 0, "loadGridFromFile return ko if path NULL");

    res = saveGridToFile(NULL);
    ASSERT(res == 0, "saveGridToFile return ko if path NULL");
}

static void testSubset(void) {
    printf("\n[TEST] subset\n");
}

int main(void)
{
    /*
    testInitGrid();
    testSetTileValue();
    testDisp();
    testIoFile(); */

    cli();

    loadGridFromFile("filebase/sudoku_test.txt");
    dispFinal();
    resolveGrid();
    dispFinal();

    /*
    dispPossible();
    buildAllSubsets();
    cleanGrid();
    dispSubset(getLineSubset(1));
    dispFinal();
    resolveGrid();
    dispFinal();
    isGridValid(); */


    printf("  \nResults : %d/%d passed", passed, run);
    if (failed > 0) {
        printf("  (%d FAILED)\n", failed);
    }

    return (failed == 0) ? 0 : 1;
}