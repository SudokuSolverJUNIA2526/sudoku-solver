//
// Created by Xan Delayat on 27/05/2026.
//

#include "grid.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "sudoku_types.h"

// initialize empty grid (all the values are unknown, all possible)
void initGrid(void) {
   for (int i = 0; i < 81; i++) {
       grid[i].value = 0;
       for (int j = 0; j < 9; j++) {
           grid[i].possible[j] = 0;         //0 => unknown value ; 1 => known value (change to proceed tests)
       }
   }
}

/**
 * display grid and indicate only certain values
 * Empty tiles are displayed as a space " "
 * Displaying must clearly show limitations between each sub-squares.
**/
void dispFinal(void) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            printf("[");
            grid[i].possible[j] == 0 ? printf(" ") : printf("%i", grid[i].value);
            printf("]");
            if (j+1 % 3 == 0) {
                printf("|");
            } else if (j == 8) {
                printf("\n");
                if (i+1 % 3 == 0) {
                    for (int k = 0; k < 9; k++) {
                        printf("---");
                        if (j+1 % 3 == 0) {
                            printf("|");
                        }
                    }
                }
            }
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

}
