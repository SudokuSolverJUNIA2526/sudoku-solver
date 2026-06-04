//
// Created by Xan Delayat on 27/05/2026.
//

#ifndef SUDOKUSOLVER_GENERATOR_H
#define SUDOKUSOLVER_GENERATOR_H

#include "sudoku_types.h"
#include "generator-internal.h"

char generateGrid(difficulty_t level, unsigned int seed);

#endif //SUDOKUSOLVER_GENERATOR_H