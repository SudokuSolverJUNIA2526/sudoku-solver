//
// Created by Xan Delayat on 27/05/2026.
//

#ifndef SUDOKUSOLVER_CLI_H
#define SUDOKUSOLVER_CLI_H

#include "io.h"
#include "../entity/grid.h"

static void printHelp(const char *cmd);
char cli();
static void generateLevelHelp();
unsigned short seedGen(unsigned short seed);
char verbose();
char benchmark();
char interactive();

#endif //SUDOKUSOLVER_CLI_H