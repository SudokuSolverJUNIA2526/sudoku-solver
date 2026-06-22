//
// Created by Xan Delayat on 27/05/2026.
//

#ifndef SUDOKUSOLVER_SUDOKU_TYPES_H
#define SUDOKUSOLVER_SUDOKU_TYPES_H

#define GRID_SIZE 81
#define HISTORY_SIZE 81

typedef struct SudokuTiles {
    char value;                 /** 0 if unknown, otherwise 1..9 **/
    char possible[9];           /** possible[i] != 0 if value i+1 is possible, 9 values possible **/
} SudokuTiles;

extern SudokuTiles grid[81];
/**
 * Tile from j column and i row (with 1<=i,j<=9) is set by grid[(i-1)*9 + (j-1)]
 **/

typedef SudokuTiles **Subset;

typedef struct {
    SudokuTiles *tile;     // case modifiée
    char         supposed; // 0 = déduction, 1 = supposition
    char         value;
} Affectation;

typedef struct {
    unsigned short difficulty;  // de 1 à 5
} difficulty_t;

extern char g_verbose;

#endif //SUDOKUSOLVER_SUDOKU_TYPES_H