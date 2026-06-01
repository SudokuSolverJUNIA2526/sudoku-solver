//
// Created by Xan Delayat on 27/05/2026.
//

#include "cli.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "stdlib.h"
#include "../console/cli.h"
#include "../repository/io.h"

extern SudokuTiles grid[81];
extern Affectation history[81];
extern int history_index;

    static void printHelp(const char *prog)
{
    fprintf(stderr,
            "Usage: %s [-h] [-l FILE] [-i]\n"
            "  -h, --help          Display this help interface\n"
            "  -l, --load FILE     Load and display a grid from FILE\n"
            "  -i, --interactive   Input a grid\n",
            prog);
}

int cli(int argc, char *argv[])
{
    if (argc < 2) {
        printHelp(argv[0]);
        return 1;
    }

    // comparer les entrées de commandes
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printHelp(argv[0]);
            return 0;

        } else if ((strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--load") == 0)
                   && i + 1 < argc) {
            i++;
            printf("Loading '%s'...\n", argv[i]);
            if (!loadGridFromFile(argv[i])) {
                fprintf(stderr, "Loading error.\n");
                return 1;
            }
            printf("\n--- Loaded grid (known values) ---\n");
            dispFinal();
            printf("\n--- Possibilities grid ---\n");
            dispPossible();

                   } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interactive") == 0) {
                       reqStartGrid();
                       printf("\n--- Grid ---\n");
                       dispFinal();

                   } else {
                       fprintf(stderr, "Unknown command : %s\n", argv[i]);
                       printHelp(argv[0]);
                       return 1;
                   }
    }

    return 0;
}
