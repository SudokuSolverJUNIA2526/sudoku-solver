#include <stdio.h>
#include <string.h>

#include "../entity/grid-internal.h"
#include "../entity/sudoku_types.h"
#include "../repository/io.h"
#include "../entity/grid.h"
#include "../console/cli.h"
#include "../entity/generator.h"
#include "../entity/subset.h"
#include "../service/service_solver.h"
#include <windows.h>

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

static void testPointingPairs(void) {
    printf("\n[TEST] cleanPointingPairs\n");

    /*
     * cas normal :
     * dans le sous-carre 1 (lignes 1-3, colonnes 1-3), la valeur 5
     * n'est possible qu'en ligne 1 (cases (1,1) et (1,2)).
     * apres cleanPointingPairs, la valeur 5 doit etre retiree des
     * autres cases de la ligne 1 hors du sous-carre (colonnes 4 a 9).
     */
    initGrid();
    buildAllSubsets();

    /* on retire 5 de toutes les cases du sous-carre 1 sauf (1,1) et (1,2) */
    for (int i = 0; i < 81; i++) grid[i].possible[4] = 0;
    grid[0].possible[4] = 1; /* (1,1) */
    grid[1].possible[4] = 1; /* (1,2) */
    /* on laisse 5 possible en (1,5) hors du sous-carre */
    grid[4].possible[4] = 1; /* (1,5) */

    char modified = cleanPointingPairs();

    ASSERT(modified == 1, "pointing pairs : returns 1 when modification");
    ASSERT(grid[4].possible[4] == 0,
           "pointing pairs : value 5 removed from (1,5) outside sub-square");
    ASSERT(grid[0].possible[4] != 0,
           "pointing pairs : (1,1) inside sub-square not modified");
    ASSERT(grid[1].possible[4] != 0,
           "pointing pairs : (1,2) inside sub-square not modified");

    /* cas limite : aucune pointing pair, rien ne doit changer */
    initGrid();
    modified = cleanPointingPairs();
    ASSERT(modified == 0, "pointing pairs : returns 0 if nothing to do");

    freeAllSubsets();
}

static void testBoxLineReduction(void) {
    printf("\n[TEST] cleanBoxLineReduction\n");

    /*
     * cas normal :
     * sur la ligne 1, la valeur 7 n'est possible que dans les
     * colonnes 1, 2 et 3 (sous-carre 1). apres cleanBoxLineReduction,
     * la valeur 7 doit etre retiree des autres cases du sous-carre 1
     * (lignes 2 et 3, colonnes 1-3).
     */
    initGrid();
    buildAllSubsets();

    for (int i = 0; i < 81; i++) grid[i].possible[6] = 0;
    grid[0].possible[6] = 1; // (1,1)
    grid[1].possible[6] = 1; // (1,2)
    grid[2].possible[6] = 1; // (1,3)
    // valeur 7 aussi possible en (2,1) dans le meme sous-carre
    grid[9].possible[6] = 1;  /* (2,1) */

    char modified = cleanBoxLineReduction();

    ASSERT(modified == 1, "box-line : returns 1 when modification");
    ASSERT(grid[9].possible[6] == 0, "box-line : value 7 removed from (2,1) outside the line");
    ASSERT(grid[0].possible[6] != 0, "box-line : (1,1) on the line not modified");

    /* cas limite : aucune reduction possible */
    initGrid();
    modified = cleanBoxLineReduction();
    ASSERT(modified == 0, "box-line : returns 0 if nothing to do");

    freeAllSubsets();
}

static void testXWing(void) {
    printf("\n[TEST] cleanXWing\n");

    /*
     * cas normal :
     * la valeur 3 n'est possible que dans les colonnes 2 et 5
     * sur les lignes 1 et 4. c'est un X-Wing : on retire 3 de
     * toutes les autres cases des colonnes 2 et 5.
     */
    initGrid();
    buildAllSubsets();

    for (int i = 0; i < 81; i++) grid[i].possible[2] = 0;
    grid[1].possible[2]  = 1; /* (1,2) */
    grid[4].possible[2]  = 1; /* (1,5) */
    grid[28].possible[2] = 1; /* (4,2) */
    grid[31].possible[2] = 1; /* (4,5) */
    /* valeur 3 aussi possible en (7,2) et (7,5) : doit etre retiree */
    grid[55].possible[2] = 1; /* (7,2) */
    grid[58].possible[2] = 1; /* (7,5) */

    char modified = cleanXWing();

    ASSERT(modified == 1, "x-wing : returns 1 when modification");
    ASSERT(grid[55].possible[2] == 0, "x-wing : value 3 removed from (7,2)");
    ASSERT(grid[58].possible[2] == 0, "x-wing : value 3 removed from (7,5)");
    ASSERT(grid[1].possible[2]  != 0, "x-wing : (1,2) from pattern not modified");
    ASSERT(grid[31].possible[2] != 0, "x-wing : (4,5) from pattern not modified");

    /* cas limite : pas de x-wing, rien ne change */
    initGrid();
    modified = cleanXWing();
    ASSERT(modified == 0, "x-wing : returns 0 if nothing to do");

    freeAllSubsets();
}

static void testSwordfish(void) {
    printf("\n[TEST] cleanSwordfish\n");

    /*
     * cas normal :
     * la valeur 6 n'est possible que dans les colonnes 1, 4 et 7
     * sur les lignes 1, 4 et 7. c'est un swordfish : on retire 6
     * de toutes les autres cases de ces 3 colonnes.
     */
    initGrid();
    buildAllSubsets();

    for (int i = 0; i < 81; i++) grid[i].possible[5] = 0;

    /* ligne 1 : colonnes 1, 4, 7 */
    grid[0].possible[5]  = 1; /* (1,1) */
    grid[3].possible[5]  = 1; /* (1,4) */
    grid[6].possible[5]  = 1; /* (1,7) */
    /* ligne 4 : colonnes 1, 4, 7 */
    grid[27].possible[5] = 1; /* (4,1) */
    grid[30].possible[5] = 1; /* (4,4) */
    grid[33].possible[5] = 1; /* (4,7) */
    /* ligne 7 : colonnes 1, 4, 7 */
    grid[54].possible[5] = 1; /* (7,1) */
    grid[57].possible[5] = 1; /* (7,4) */
    grid[60].possible[5] = 1; /* (7,7) */
    /* valeur 6 aussi possible en (2,1) : doit etre retiree */
    grid[9].possible[5]  = 1; /* (2,1) */

    char modified = cleanSwordfish();

    ASSERT(modified == 1, "swordfish : returns 1 when modification");
    ASSERT(grid[9].possible[5] == 0, "swordfish : value 6 removed from (2,1) outside pattern");
    ASSERT(grid[0].possible[5] != 0, "swordfish : (1,1) from pattern not modified");

    // cas limite
    initGrid();
    modified = cleanSwordfish();
    ASSERT(modified == 0, "swordfish : returns 0 if nothing to do");

    freeAllSubsets();
}

static void testJellyfish(void) {
    printf("\n[TEST] cleanJellyfish\n");

    /*
     * cas normal :
     * la valeur 2 n'est possible que dans les colonnes 1, 3, 5, 7
     * sur les lignes 1, 3, 5, 7. c'est un jellyfish : on retire 2
     * de toutes les autres cases de ces 4 colonnes.
     */
    initGrid();
    buildAllSubsets();

    for (int i = 0; i < 81; i++) grid[i].possible[1] = 0;

    /* ligne 1 : colonnes 1, 3, 5, 7 */
    grid[0].possible[1]  = 1; grid[2].possible[1]  = 1;
    grid[4].possible[1]  = 1; grid[6].possible[1]  = 1;
    /* ligne 3 : colonnes 1, 3, 5, 7 */
    grid[18].possible[1] = 1; grid[20].possible[1] = 1;
    grid[22].possible[1] = 1; grid[24].possible[1] = 1;
    /* ligne 5 : colonnes 1, 3, 5, 7 */
    grid[36].possible[1] = 1; grid[38].possible[1] = 1;
    grid[40].possible[1] = 1; grid[42].possible[1] = 1;
    /* ligne 7 : colonnes 1, 3, 5, 7 */
    grid[54].possible[1] = 1; grid[56].possible[1] = 1;
    grid[58].possible[1] = 1; grid[60].possible[1] = 1;
    /* valeur 2 aussi possible en (2,1) : doit etre retiree */
    grid[9].possible[1]  = 1; /* (2,1) */

    char modified = cleanJellyfish();

    ASSERT(modified == 1, "jellyfish : returns 1 when modification");
    ASSERT(grid[9].possible[1] == 0, "jellyfish : value 2 removed from (2,1) outside pattern");
    ASSERT(grid[0].possible[1] != 0, "jellyfish : (1,1) from pattern not modified");

    // cas limite
    initGrid();
    modified = cleanJellyfish();
    ASSERT(modified == 0, "jellyfish : returns 0 if nothing to do");

    freeAllSubsets();
}

static void testUniqueRectangle(void) {
    printf("\n[TEST] cleanUniqueRectangle\n");

    /*
     * cas normal (type 1) :
     * 4 cases forment un rectangle sur les lignes 1 et 4,
     * colonnes 1 et 4, dans 2 sous-carres differents.
     * 3 cases n'ont que {1,2} comme possibles (toits).
     * la 4e case (sol) a {1,2,5} : on doit lui retirer 1 et 2.
     */
    initGrid();
    buildAllSubsets();

    for (int i = 0; i < 81; i++)
        for (int d = 0; d < 9; d++)
            grid[i].possible[d] = 0;

    /* (1,1) toit : {1,2} */
    grid[0].possible[0] = 1; grid[0].possible[1] = 1;
    /* (1,4) toit : {1,2} */
    grid[3].possible[0] = 1; grid[3].possible[1] = 1;
    /* (4,1) toit : {1,2} */
    grid[27].possible[0] = 1; grid[27].possible[1] = 1;
    /* (4,4) sol : {1,2,5} */
    grid[30].possible[0] = 1; grid[30].possible[1] = 1;
    grid[30].possible[4] = 1;

    char modified = cleanUniqueRectangle();

    ASSERT(modified == 1, "unique rectangle : returns 1 when modification");
    ASSERT(grid[30].possible[0] == 0, "unique rectangle : value 1 removed from floor (4,4)");
    ASSERT(grid[30].possible[1] == 0, "unique rectangle : value 2 removed from floor (4,4)");
    ASSERT(grid[30].possible[4] != 0, "unique rectangle : value 5 kept in floor (4,4)");
    ASSERT(grid[0].possible[0] != 0, "unique rectangle : roof (1,1) not modified");
    ASSERT(modified == 0, "unique rectangle : returns 0 if nothing to do");

    // cas limite : aucun rectangle unique
    initGrid();
    modified = cleanUniqueRectangle();
    ASSERT(modified == 0, "unique rectangle : returns 0 if nothing to do");

    freeAllSubsets();
}

static void testYWing(void) {
    printf("\n[TEST] cleanYWing\n");

    /*
     * cas normal :
     * pivot  (1,1) : {1,2}
     * pince1 (1,5) : {1,3}  <- visible par le pivot (meme ligne)
     * pince2 (5,1) : {2,3}  <- visible par le pivot (meme colonne)
     * toute case visible a la fois par (1,5) et (5,1) doit perdre 3.
     * (5,5) est visible par (1,5) (meme colonne 5) et par (5,1)
     * (meme ligne 5) : elle doit perdre 3.
     */
    initGrid();
    buildAllSubsets();

    for (int i = 0; i < 81; i++)
        for (int d = 0; d < 9; d++)
            grid[i].possible[d] = 0;

    /* pivot (1,1) : {1,2} */
    grid[0].possible[0] = 1;
    grid[0].possible[1] = 1;

    /* pince1 (1,5) : {1,3} */
    grid[4].possible[0] = 1;
    grid[4].possible[2] = 1;

    /* pince2 (5,1) : {2,3} */
    grid[36].possible[1] = 1;
    grid[36].possible[2] = 1;

    /* cible (5,5) visible par pince1 et pince2 : {3,7} */
    grid[40].possible[2] = 1;
    grid[40].possible[6] = 1;

    char modified = cleanYWing();

    ASSERT(modified == 1, "y-wing : returns 1 when modification");
    ASSERT(grid[40].possible[2] == 0, "y-wing : value 3 removed from (5,5)");
    ASSERT(grid[40].possible[6] != 0, "y-wing : value 7 kept in (5,5)");
    ASSERT(grid[0].possible[0] != 0,  "y-wing : pivot (1,1) not modified");
    ASSERT(grid[4].possible[2] != 0,  "y-wing : wing1 (1,5) not modified");
    ASSERT(grid[36].possible[2] != 0, "y-wing : wing2 (5,1) not modified");

    // cas limite : aucun y-wing
    initGrid();
    modified = cleanYWing();
    ASSERT(modified == 0, "y-wing : returns 0 if nothing to do");

    freeAllSubsets();
}

char testInit(void) {
    testInitGrid();
    testSetTileValue();
    testDisp();
    testIoFile();

    testPointingPairs();
    testBoxLineReduction();
    testXWing();
    testSwordfish();
    testJellyfish();
    testUniqueRectangle();
    testYWing();

    fprintf(stdout, "  \nResults : %d/%d passed", passed, run);
    if (failed > 0) {
        fprintf(stderr, "(%d FAILED)\n\n", failed);
    }
    fprintf(stdout, "\n\n");
    Sleep(1000);

    return (failed == 0) ? 0 : 1;
}