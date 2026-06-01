//
// Created by Xan Delayat on 27/05/2026.
//

#include "grid.h"

#include <stdio.h>
#include <stdlib.h>

SudokuTiles grid[81];
Affectation history[81];
int history_index = 0;

// initialize empty grid (all the values are unknown, all possible)
void initGrid(void)
{
    for (int k = 0; k < GRID_SIZE; k++) {
        grid[k].value = 0;
        for (int d = 0; d < 9; d++) {
            grid[k].possible[d] = 1;
        }
    }
}

/**
 * display grid and indicate only certain values
 * Empty tiles are displayed as a space " "
 * Displaying must clearly show limitations between each sub-squares.
**/

void dispFinal(void) {
    SudokuTiles *t;

    printf("+-------+-------+-------+\n");
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            if (j == 1 || j == 4 || j == 7) {
                printf("| ");
            }
            t = &grid[(i - 1) * 9 + (j - 1)];
            if (t->value != 0) {
                printf("%c ", '0' + t->value);
            } else {
                printf("  ");
            }
        }
        printf("|\n");
        if (i == 3 || i == 6 || i == 9) {
            printf("+-------+-------+-------+\n");
        }
    }
}

    // display 9 grids, one per possible value: d belongs to [1,9]
    /**
     * For each case (function above):
     * if tile's value is known and = d, then display d;
     * if value is known and != d, then display .;
     * if value is unknown and d is possible, then display +;
     * if value is unknown and d is impossible, then display a space " ".
     **/
    void dispPossible(void) {
        SudokuTiles *t;

        for (int d = 1; d <= 9; d++) {
            printf("Possibilities for: %d\n", d);
            printf("+-------+-------+-------+\n");
            for (int i = 1; i <= 9; i++) {
                for (int j = 1; j <= 9; j++) {
                    if (j == 1 || j == 4 || j == 7) {
                        printf("| ");
                    }
                    t = &grid[(i - 1) * 9 + (j - 1)];
                    if (t->value != 0) {
                        if (t->value == (char)d) {
                            printf("%c ", '0' + d);
                        } else {
                            printf(". ");
                        }
                    } else {
                        if (t->possible[d - 1] != 0) {
                            printf("+ ");
                        } else {
                            printf("  ");
                        }
                    }
                }
                printf("|\n");
                if (i == 3 || i == 6 || i == 9) {
                    printf("+-------+-------+-------+\n");
                }
            }
            printf("\n");
        }
    }

void setTileValue(SudokuTiles *t, char val, char supposed)
{
    t->value = val;

    for (int d = 0; d < 9; d++) {
        t->possible[d] = 0;
    }
    if (val >= 1 && val <= 9) {
        t->possible[val - 1] = 1;
    }

    if (history_index < HISTORY_SIZE) {
        history[history_index].tile     = t;
        history[history_index].supposed = supposed;
        history[history_index].value    = val;
        history_index++;
    }
}