//
// Created by Xan Delayat on 27/05/2026.
//

#include "service_solver.h"
#include "../entity/grid.h"


/**
typedef struct SudokuTiles {
   char value;
   char possible[9];
} SudokuTiles;
**/

char cleanLine(SudokuTiles *line) {
    int modificator = 0;
    for (int i = 0; i < 9; i++) {
        if (line[i].value != 0) {       // Check si tile value != 0
            for (int j = 0; j < 9; j++) {
                if (line[j].value != 0) {
                    //NA
                } else {
                    line[j].possible[line[i].value] = 0;
                    modificator++;
                }
            }
        }
    }
    if (modificator != 0) return 1;
    return 0;
}

char solveNakedSingles(void) {
    int possible_count = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        possible_count = 0;
        for (int j = 0; j < 9; j++) {
            if (grid[i].value != 0 && grid[i].possible[j+1] == 0) {
                possible_count++;
            }
        }
        if (possible_count == 8) {
            for (int j = 0; j < 9; j++) {
                if (grid[i].possible[j+1] != 0) {
                    j++;
                    grid[i].value = (char) j;
                    grid[i].possible[j+1] = 0;
                    j--;
                }
            }
        }
    }
    return 1;
}

char solveHiddenSinglesInLine(SudokuTiles *line) {
    int modifier = 0;
    int possible_count = 0;
    for (int i = 0; i < 9; i++) {       // check every tiles
        for (int v = 0; v < 9; v++) {       // check each values
            // si valeur déjà fixée et possible, incrementer compteur
            if (line[i].value != 0 && line[i].possible[v+1] == 0)
                possible_count++;
        }
        // si une seule case possible pour valeur v (compteur à 8)
        if (possible_count == 8) {
            for (int v = 0; v < 9; v++) {
                if (line[i].possible[v+1] != 0) {
                    line[i].value = (char) v;
                    modifier++;
                }
            }
        }
    }
    if (modifier != 0) return 1;
    return 0;
}
