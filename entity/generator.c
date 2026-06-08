//
// Created by Xan Delayat on 27/05/2026.
//

#include "generator.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "stdlib.h"
#include "subset.h"
#include "../console/cli.h"
#include "sudoku_types.h"

char generateGrid(difficulty_t level, unsigned int seed) {
    initGrid();
    buildAllSubsets();
    srand(seed);

    //ligne 1 : valeurs 1 à 9 dans un ordre aléatoire
    int vals[9] = {1,2,3,4,5,6,7,8,9};

    for (int i = 8; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = vals[i]; vals[i] = vals[j]; vals[j] = tmp;
    }
    for (int j = 0; j < 9; j++)
        setTileValue(&grid[j], (char)vals[j], 0);

    // lignes 2 à 9 : décalage circulaire
    int shifts[8] = {3,6,1,4,7,2,5,8};
    for (int i = 0; i < 8; i++) {
        int shift = shifts[i];
        for (int j = 0; j < 9; j++) {
            int src = vals[(j + shift) % 9];
            setTileValue(&grid[(i+1)*9 + j], (char)src, 0);
        }
    }
    return 1;
}
