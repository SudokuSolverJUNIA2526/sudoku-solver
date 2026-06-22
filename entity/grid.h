//
// Created by Xan Delayat on 27/05/2026.
//

#ifndef SUDOKUSOLVER_GRID_H
#define SUDOKUSOLVER_GRID_H

#include "sudoku_types.h"
#include "../console/cli-internal.h"

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

void setTileValue(SudokuTiles *tile, char val, char supposed);

char isGridValid(void);
void back_play(void);

extern SudokuTiles grid[GRID_SIZE];
extern Affectation history[HISTORY_SIZE];
extern int  history_index;

typedef void (*TraceCallback)(int row, int col, char val, char supposed);
void setTraceCallback(TraceCallback callback);

#endif //SUDOKUSOLVER_GRID_H