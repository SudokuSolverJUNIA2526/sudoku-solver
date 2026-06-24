//
// Created by Xan Delayat on 27/05/2026.
//

#ifndef SUDOKUSOLVER_CLI_H
#define SUDOKUSOLVER_CLI_H

#include "io.h"
#include "../entity/grid.h"
#include "cli-internal.h"
#include "../entity/generator.h"
#include "../repository/io.h"
#include "../entity/sudoku_types.h"
#include "../service/service_solver.h"

static void printHelp(const char *cmd);
char cli();
static void generateLevelHelp();
char verbose();
static void traceVerbose(int row, int col, char val, char supposed);
char benchmark();
char interactive();

#endif //SUDOKUSOLVER_CLI_H