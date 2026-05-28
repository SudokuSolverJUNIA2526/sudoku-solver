//
// Created by Xan Delayat on 27/05/2026.
//

#ifndef SUDOKUSOLVER_SUDOKU_TYPES_H
#define SUDOKUSOLVER_SUDOKU_TYPES_H

typedef struct SudokuTiles {
    char value;                 /** 0 if unknown, otherwise 1..9 **/
    char possible[9];           /** possible[i] != 0 if value i+1 is possible, 9 values possible **/
} SudokuTiles;

SudokuTiles grid[81];
/**
 * Tile from j column and i row (with 1<=i,j<=9) is set by grid[(i-1)*9 + (j-1)]
 **/

typedef SudokuTiles **Subset;

#endif //SUDOKUSOLVER_SUDOKU_TYPES_H