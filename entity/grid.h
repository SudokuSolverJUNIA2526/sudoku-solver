//
// Created by Xan Delayat on 27/05/2026.
//

#ifndef SUDOKUSOLVER_GRID_H
#define SUDOKUSOLVER_GRID_H

void initGrid(void);                // initialize empty grid (all the values are unknown, all possible)
void dispFinal(void);               // display grid and indicate only certain values. Empty tiles are displayed as a space " ". Displaying must clearly show limitations between each sub-squares.
void dispPossible(void);            // display 9 grids, one per possible value: d belongs to [1,9]
/**
 * For each case (function above):
 * if tile's value is known and = d, then display d;
 * if value is known and != d, then display .;
 * if value is unknown and d is possible, then display +;
 * if value is unknown and d is impossible, then display a space " ".
 **/

#endif //SUDOKUSOLVER_GRID_H