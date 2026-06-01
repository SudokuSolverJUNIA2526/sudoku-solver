//
// Created by Xan Delayat on 27/05/2026.
//

#ifndef SUDOKUSOLVER_IO_H
#define SUDOKUSOLVER_IO_H

#include "../entity/grid.h"

char loadGridFromFile(const char *path);
char saveGridToFile(const char *path);
void reqStartGrid(void);

#endif //SUDOKUSOLVER_IO_H