#include "service_solver.h"
#include "../entity/grid.h"
#include "../entity/subset.h"

/* ------------------------------------------------------------------ */
/* cleanLine                                                            */
/* ------------------------------------------------------------------ */

/*
 * Pour chaque case de valeur connue dans la ligne,
 * marque cette valeur comme impossible dans les autres cases inconnues.
 */
char cleanLine(SudokuTiles *line) {
    int modified = 0;
    for (int i = 0; i < 9; i++) {
        if (line[i].value == 0) continue;
        int val = line[i].value - 1; /* index 0-based */
        for (int j = 0; j < 9; j++) {
            if (i == j) continue;
            if (line[j].value != 0) continue;
            if (line[j].possible[val] != 0) {
                line[j].possible[val] = 0;
                modified = 1;
            }
        }
    }
    return (char)modified;
}

/* ------------------------------------------------------------------ */
/* solveNakedSingles                                                    */
/* ------------------------------------------------------------------ */

/*
 * Parcourt toutes les cases inconnues de la grille.
 * Si une case n'a plus qu'une seule valeur possible, on la fixe.
 */
char solveNakedSingles(void) {
    int modified = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        if (grid[i].value != 0) continue; /* case déjà connue */
        int count = 0, last = -1;
        for (int v = 0; v < 9; v++) {
            if (grid[i].possible[v] != 0) { count++; last = v; }
        }
        if (count == 1) {
            setTileValue(&grid[i], (char)(last + 1), 0);
            modified = 1;
        }
    }
    return (char)modified;
}

/* ------------------------------------------------------------------ */
/* solveHiddenSinglesInLine                                             */
/* ------------------------------------------------------------------ */

/*
 * Pour chaque valeur v (1..9), si v n'est possible que dans une unique
 * case inconnue de la ligne, cette case prend v.
 */
char solveHiddenSinglesInLine(SudokuTiles *line) {
    int modified = 0;
    for (int v = 1; v <= 9; v++) {
        int count = 0, last = -1;
        for (int i = 0; i < 9; i++) {
            if (line[i].value != 0) continue;
            if (line[i].possible[v-1] != 0) { count++; last = i; }
        }
        if (count == 1) {
            setTileValue(&line[last], (char)v, 0);
            modified = 1;
        }
    }
    return (char)modified;
}

/* ------------------------------------------------------------------ */
/* resolveGrid                                                          */
/* ------------------------------------------------------------------ */

/*
 * Boucle de résolution complète.
 * Enchaîne toutes les techniques jusqu'à ce qu'il n'y ait plus de progrès.
 * Retourne 1 si la grille est entièrement résolue, 0 sinon.
 */
char resolveGrid(void) {
    int modified;
    do {
        modified  = cleanGrid();
        modified |= solveNakedSingles();
        modified |= solveHiddenSingles();
        modified |= solveNakedSingles();
        modified |= cleanNakedPairs();
        modified |= cleanHiddenPairs();
        modified |= cleanNakedTriples();
        modified |= cleanHiddenTriples();
        modified |= solveNakedSingles();
        modified |= solveHiddenSingles();
    } while (modified);

    // Vérifie si la grille est entièrement résolue
    for (int i = 0; i < GRID_SIZE; i++)
        if (grid[i].value == 0) return 0;
    return 1;
}

