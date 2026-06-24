#include <stdio.h>
#include <string.h>

#include "../sudoku-solver/entity/grid-internal.h"
#include "../sudoku-solver/entity/sudoku_types.h"
#include "../sudoku-solver/repository/io.h"
#include "../sudoku-solver/entity/grid.h"
#include "console/cli.h"
#include "entity/generator.h"
#include "entity/subset.h"
#include "service/service_solver.h"
#include "../sudoku-solver/test/tests.c"

int main(void)
{
    testInit();       // lance les tests

    cli();

    loadGridFromFile("filebase/sudoku_test.txt");
    dispFinal();
    resolveGrid();
    dispFinal();

    return 0;
}