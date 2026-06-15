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

    // si seed == 0 => NULL
    if (seed == 0)
        srand((unsigned int)time(NULL));
    else
        srand(seed);

    //grille de base valide (décalage circulaire)
    int vals[9] = {1,2,3,4,5,6,7,8,9};

    // mélange de la ligne 1
    for (int i = 8; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = vals[i]; vals[i] = vals[j]; vals[j] = tmp;
    }

    int shifts[9] = {0,3,6,1,4,7,2,5,8};
    for (int i = 0; i < 9; i++) {
        int shift = shifts[i];
        for (int j = 0; j < 9; j++) {
            int src = vals[(j + shift) % 9];
            setTileValue(&grid[i*9 + j], (char)src, 0);
        }
    }

    // mélange des lignes à l'intérieur de chaque groupe de 3
    // permuter les lignes dans chaque bande (0-2, 3-5, 6-8) préserve
    // les sous-carrés et les colonnes
    for (int band = 0; band < 3; band++) {
        for (int i = 2; i > 0; i--) {
            int j = rand() % (i + 1);
            // échange les lignes band*3+i et band*3+j
            int r1 = band*3 + i;
            int r2 = band*3 + j;
            for (int col = 0; col < 9; col++) {
                char tmp = grid[r1*9 + col].value;
                grid[r1*9 + col].value = grid[r2*9 + col].value;
                grid[r2*9 + col].value = tmp;
            }
        }
    }

    // mélange des colonnes à l'intérieur de chaque groupe de 3
    for (int stack = 0; stack < 3; stack++) {
        for (int i = 2; i > 0; i--) {
            int j = rand() % (i + 1);
            int c1 = stack*3 + i;
            int c2 = stack*3 + j;
            for (int row = 0; row < 9; row++) {
                char tmp = grid[row*9 + c1].value;
                grid[row*9 + c1].value = grid[row*9 + c2].value;
                grid[row*9 + c2].value = tmp;
            }
        }
    }

    // retrait des cases selon le niveau
    // de 1 (trivial) à 5 (expert)
    int nb_remove;
    switch (level.difficulty) {
        case 1:
            nb_remove = 30;
            break;
        case 2:
            nb_remove = 40;
            break;
        case 3:
            nb_remove = 50;
            break;
        case 4:
            nb_remove = 60;
            break;
        default:
            nb_remove = 70;
            break;
    }

    int removed = 0;
    int attempts = 0;
    while (removed < nb_remove && attempts < 1000) {
        int index_rm = rand() % 81;
        attempts++;
        // cherche une case encore remplie
        if (grid[index_rm].value == 0) continue;
        grid[index_rm].value = 0;
        for (int d = 0; d < 9; d++)
            grid[index_rm].possible[d] = 1;
        removed++;
    }

    return 1;
}
