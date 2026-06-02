//
// Created by Xan Delayat on 27/05/2026.
//

#include "subset.h"

#include <stdlib.h>

// sous-ensemble correspondant à la ligne n
Subset getLineSubset(int n) {
    Subset subset = malloc(sizeof(SudokuTiles *) * 9);
    if (n > 9) n = 9;
    for (int i = 0; i < 9; i++) {
        // (n-1) -> numéro de ligne (0 <= n <= 8)
        // * 9 -> corresponcance valeurs de la ligne
        // + i -> index de la ligne
        subset[i] = &grid[(n - 1) * 9 + i];
    }
    return subset;
}

// sous-ensemble correspondant à la colonne n
Subset getColSubset(int n) {
    Subset subset = malloc(sizeof(SudokuTiles *) * 9);
    if (n > 9) n = 9;
    for (int i = 0; i < 9; i++) {
        // (n-1) -> numéro de colonne (0 <= n <= 8)
        // * i -> corresponcance valeurs de la ligne
        // + 9 -> index de la colonne
        subset[i] = &grid[(n - 1) * i + 9];
    }
    return subset;
}

// sous-ensemble correspondant au sous-carré n. Informez
// clairement votre schéma de numérotation dans la documentation.
Subset getSubsqSubset(int n) {
    Subset subset = malloc(sizeof(SudokuTiles *) * 9);
    int start_row = ((n - 1) / 3) * 3;
    int start_col = ((n - 1) % 3) * 3;
    for (int i = 0; i < 9; i++) {
        int row = start_row + (i / 3);
        int col = start_col + (i % 3);
        subset[i] = &grid[row * 9 + col];
    }
    return subset;
}

// construit les 27 sous-ensembles (9 lignes + 9 colonnes
// + 9 sous-carrés) et les mémorise dans une structure
// dont vous justifierez le type
void buildAllSubsets(void) {
    SubsetGrid *subsets = malloc(sizeof(Subset*) * 9);
    for (int i = 0; i < 9; i++) {
        subsets[i].col = getColSubset(i);
        subsets[i].line = getLineSubset(i);
        subsets[i].sub_square = getSubsqSubset(i);
    }
    free(subsets);
}

// versions généralisées de clean_line et solve_hidden_singles_in_line
char cleanSubset(Subset s) {
    int modificator = 0;
    for (int i = 0; i < 9; i++) {
        if (s[i]->value != 0) {       // Check si tile value != 0
            for (int j = 0; j < 9; j++) {
                if (s[j]->value != 0) {
                    //NA
                } else {
                    s[j]->possible[s[i]->value] = 0;
                    modificator++;
                }
            }
        }
    }
    if (modificator != 0) return 1;
    return 0;
}

// versions généralisées de clean_line et solve_hidden_singles_in_line
char solveHiddenSinglesInSubset(Subset s) {
    int possible_count = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        possible_count = 0;
        for (int j = 0; j < 9; j++) {
            if (s[i]->value != 0 && s[i]->possible[j+1] == 0) {
                possible_count++;
            }
        }
        if (possible_count == 8) {
            for (int j = 0; j < 9; j++) {
                if (s[i]->possible[j+1] != 0) {
                    j++;
                    s[i]->value = (char) j;
                    s[i]->possible[j+1] = 0;
                    j--;
                }
            }
        }
    }
    return 1;
}

// appliquent les fonctions
// précédentes à l’ensemble des sous-ensembles.
char cleanGrid(void) {
    for (int i = 0; i < 9; i++) {
        cleanSubset(getLineSubset(i));
        cleanSubset(getColSubset(i));
        cleanSubset(getSubsqSubset(i));
    }
    return 1;
}

// appliquent les fonctions
// précédentes à l’ensemble des sous-ensembles.
char solveHiddenSingles(void) {
    for (int i = 0; i < 9; i++) {
        solveHiddenSinglesInSubset(getLineSubset(i));
        solveHiddenSinglesInSubset(getColSubset(i));
        solveHiddenSinglesInSubset(getSubsqSubset(i));
    }
    return 1;
}

// affichage d’un sous-ensemble (format libre, à documenter)
void dispSubset(Subset s) {

}