//
// Created by Xan Delayat on 27/05/2026.
//

#ifndef SUDOKUSOLVER_SUBSET_H
#define SUDOKUSOLVER_SUBSET_H

#include "sudoku_types.h"
#include "subset-internal.h"

Subset getLineSubset(int n);
Subset getColSubset(int n);
Subset getSubsqSubset(int n);
void buildAllSubsets(void);
char cleanSubset(Subset s);
char solveHiddenSinglesInSubset(Subset s);
char cleanGrid(void);
char solveHiddenSingles(void);
void dispSubset(Subset s);

#endif //SUDOKUSOLVER_SUBSET_H
