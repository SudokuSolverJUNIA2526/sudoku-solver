#include "service_solver.h"

#include <stdio.h>

#include "../entity/grid.h"
#include "../entity/subset.h"

/*
 * Pour chaque case de valeur connue dans la ligne,
 * marque cette valeur comme impossible dans les autres cases inconnues.
 */
char cleanLine(SudokuTiles *line) {
    int modified = 0;
    // balayer la ligne
    for (int i = 0; i < 9; i++) {
        if (line[i].value == 0) continue;
        int val = line[i].value - 1;  // index 0 based
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

/*
 * Parcourt toutes les cases inconnues de la grille.
 * Si une case n'a plus qu'une seule valeur possible, on la fixe.
 */
char solveNakedSingles(void) {
    int modified = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        if (grid[i].value != 0) continue; // case déjà connue
        int row = i / 9 + 1, col = i % 9 + 1;
        int count = 0, last = -1;
        // si possible à index i est à true
        for (int v = 0; v < 9; v++) {
            if (grid[i].possible[v] != 0) { count++; last = v; }
        }
        // si 1 seule case possible affecter une valeur automatiquement
        if (count == 1) {
            setTileValue(&grid[i], (char)(last + 1), 0);
            cleanGrid();
            modified = 1;
            i = -1;
        }
    }
    return (char)modified;
}
/*
 * Pour chaque valeur v (1 à 9), si v n'est possible que dans une unique
 * case inconnue de la ligne, cette case prend v.
 */
char solveHiddenSinglesInLine(SudokuTiles *line) {
    int modified = 0;
    // chercher sur la ligne
    for (int v = 1; v <= 9; v++) {
        int count = 0, last = -1;
        // balayer les valeurs à l'index v de la ligne
        for (int i = 0; i < 9; i++) {
            if (line[i].value != 0) continue;
            if (line[i].possible[v-1] != 0) { count++; last = i; }
        }
        // si une seule case == 0 alors setTileValue à l'index v de la ligne
        if (count == 1) {
            setTileValue(&line[last], (char)v, 0);
            modified = 1;
        }
    }
    return (char)modified;
}

/*
 * enchaîne toutes les techniques de résolution jusqu'au maximum
 * Retourne 1 si la grille est entièrement résolue, 0 sinon
 */
char resolveGrid(void) {
    int modified;
    do {
        modified = 0;
        modified |= cleanGrid();
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

    for (int i = 0; i < GRID_SIZE; i++) {
        if (grid[i].value == 0) {
            int row = i / 9 + 1;
            int col = i % 9 + 1;
            printf("Case vide : ligne %d, colonne %d, possibles : ", row, col);
            for (int v = 0; v < 9; v++)
                if (grid[i].possible[v]) printf("%d ", v+1);
            printf("\n");
        }
    }

    for (int i = 0; i < GRID_SIZE; i++) {
        if (grid[i].value == 0) {
            int row = i / 9 + 1;
            int col = i % 9 + 1;
            printf("Case vide : (%d,%d), possibles : ", row, col);
            for (int v = 0; v < 9; v++)
                if (grid[i].possible[v]) printf("%d ", v+1);
            printf("\n");
        }
    }
    return 1;
}

char guessValue(void) {
    char low_value = 0;
    char high_value = 0;
    // check dans les 81 cases de la grille
    for (int i = 0; i < GRID_SIZE; i++) {
        int possible_check = 0;
        // check les 9 possiblités de chaque case
        for (int j = 0; j < 9; j++) {
            // si valeur pas possible
            if (grid[i].possible[j] != 0 && grid[i].value == 0) {
                // possibilité + 1
                possible_check++;
                // assigner low_value tant que 1 possibilité trouvée
                if (possible_check == 1) low_value = (char) (j+1);
                if (possible_check == 2) high_value = (char) (j+1);
                if (low_value < high_value) continue;
                else low_value = high_value;
                if (possible_check == 2 && j == 8) {
                    setTileValue(&grid[i], low_value, 1);
                    return 1;
                }
            }
        }
    }
    //printf("No such type tiles");
    return 0;
}