#include "grid.h"
#include <stdio.h>
#include <string.h>

SudokuTiles grid[GRID_SIZE];
Affectation history[HISTORY_SIZE];
int         history_index = 0;

void initGrid(void) {
    for (int k = 0; k < GRID_SIZE; k++) {
        grid[k].value = 0;
        for (int d = 0; d < 9; d++)
            grid[k].possible[d] = 1;
    }
    history_index = 0;
    memset(history, 0, sizeof(history));
}

static void printHSep(void) {
    printf("+-------+-------+-------+\n");
}

void dispFinal(void) {
    printHSep();
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            if (j == 1 || j == 4 || j == 7) printf("| ");
            SudokuTiles *t = &grid[(i-1)*9 + (j-1)];
            if (t->value != 0) printf("%c ", '0' + t->value);
            else               printf("  ");
        }
        printf("|\n");
        if (i == 3 || i == 6 || i == 9) printHSep();
    }
}

void dispPossible(void) {
    for (int d = 1; d <= 9; d++) {
        printf("Possibilities for: %d\n", d);
        printHSep();
        for (int i = 1; i <= 9; i++) {
            for (int j = 1; j <= 9; j++) {
                if (j == 1 || j == 4 || j == 7) printf("| ");
                SudokuTiles *t = &grid[(i-1)*9 + (j-1)];
                if (t->value != 0) {
                    printf("%c ", t->value == (char)d ? '0'+d : '.');
                } else {
                    printf("%c ", t->possible[d-1] != 0 ? '+' : ' ');
                }
            }
            printf("|\n");
            if (i == 3 || i == 6 || i == 9) printHSep();
        }
        printf("\n");
    }
}

void setTileValue(SudokuTiles *t, char val, char supposed) {
    int row = (int)(t - grid) / 9 + 1;
    int col = (int)(t - grid) % 9 + 1;

    t->value = val;
    for (int d = 0; d < 9; d++) {
        if (t->possible[d] != 0) {
            t->possible[d] = 0;
        }
    }
    if (val >= 1 && val <= 9)
        t->possible[val - 1] = 1;

    if (history_index < HISTORY_SIZE) {
        history[history_index].tile     = t;
        history[history_index].supposed = supposed;
        history[history_index].value    = val;
        history_index++;
    }
}

char isGridValid(void) {
    int possible_check = 0;
    // check dans les 81 cases de la grille
    for (int i = 0; i < GRID_SIZE; i++) {
        // remise à 0 du compteur de possiblité restante dans une case
        possible_check = 0;
        for (int j = 0; j < 9; j++) {
            // si aucune possiblité pour valeur i et case de la valeur == 0, incrémenter le compteur
            if (grid[i].possible[j] == 0 && grid[i].value == 0) possible_check++;
            // si compteur à 9 (valeur à 0 et aucune possibilité -> impossible, return 0
            if (possible_check == 9) return 0;
        }
    }
    //printf("Valid grid\n");
    return 1;
}