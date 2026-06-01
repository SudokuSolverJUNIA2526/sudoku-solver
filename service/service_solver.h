//
// Created by Xan Delayat on 27/05/2026.
//

#ifndef SUDOKUSOLVER_SERVICE_SOLVER_H
#define SUDOKUSOLVER_SERVICE_SOLVER_H

#include "../entity/sudoku_types.h"

/**
 *
 * pour chaque valeur v déjà fixée dans une case
 * de la ligne, marque v comme impossible dans les autres cases. Retourne 1 si au moins une
 * modification a eu lieu, 0 sinon.
 */
char cleanLine(SudokuTiles *line);

/**
 * pour chaque case dont une seule valeur reste possible,
 *fixe cette valeur.
 */
char solveNakedSinglesInLine(SudokuTiles *line);

/**
 * pour chaque valeur v, si v
 * n’apparaît comme possible que dans une unique case de la ligne, alors cette case prend v
 * pour valeur.
 */
char solveHiddenSinglesInLine(SudokuTiles *line);

#endif //SUDOKUSOLVER_SERVICE_SOLVER_H