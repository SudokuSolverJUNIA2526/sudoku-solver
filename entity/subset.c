//
// Created by Xan Delayat on 27/05/2026.
//

#include "subset.h"

#include <stdio.h>
#include <stdlib.h>

Subset all_subset[27];

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
        subset[i] = &grid[i * 9 + (n - 1)];
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
    for (int i = 0; i < 9; i++) {
        all_subset[i]      = getLineSubset(i + 1);
        all_subset[9  + i] = getColSubset(i + 1);
        all_subset[18 + i] = getSubsqSubset(i + 1);
    }
}

// nettoyage de la mémoire
void freeAllSubsets(void) {
    for (int i = 0; i < 9; i++) {
        free(all_subset[i]);
        all_subset[i] = NULL;
    }
}

// versions généralisées de clean_line et solve_hidden_singles_in_line
char cleanSubset(Subset s) {
    int modified = 0;
    for (int i = 0; i < 9; i++) {
        if (s[i]->value == 0) continue; // case inconnue : rien à faire
        int val = s[i]->value - 1;      // index sur 0 dans possible[]
        for (int j = 0; j < 9; j++) {
            if (i == j) continue;                  // même case : skip
            if (s[j]->value != 0) continue;        // déjà connue : skip
            if (s[j]->possible[val] != 0) {
                s[j]->possible[val] = 0;
                modified = 1;
            }
        }
    }
    return (char)modified;
}

// versions généralisées de clean_line et solve_hidden_singles_in_line
char solveHiddenSinglesInSubset(Subset s) {
    int modified = 0;
    for (int v = 1; v <= 9; v++) {          // pour chaque valeur v
        int count = 0;
        int last  = -1;
        for (int i = 0; i < 9; i++) {
            if (s[i]->value != 0) continue; // case déjà connue
            if (s[i]->possible[v - 1] != 0) {
                    count++;
                    last = i;
            }
        }
        if (count == 1) {                   // v possible dans 1 seule case
                setTileValue(s[last], (char)v, 0);
                modified = 1;
        }
    }
    return (char)modified;
}

// appliquent les fonctions
// précédentes à l’ensemble des sous-ensembles.
char cleanGrid(void) {
    int modified = 0;
    for (int i = 0; i < 27; i++) {
        modified |= cleanSubset(all_subset[i]);
    }
    return (char)modified;
}

// appliquent les fonctions
// précédentes à l’ensemble des sous-ensembles.
char solveHiddenSingles(void) {
    int modified = 0;
    for (int i = 0; i < 27; i++) {
        modified |= solveHiddenSinglesInSubset(all_subset[i]);
    }
    return (char)modified;
}

// affichage d’un sous-ensemble (format libre, à documenter)
void dispSubset(Subset s) {
    for (int i = 0; i < 9; i++) {
        if (s[i]->value != 0) {
            printf("%d", s[i]->value);
        } else {
            int first = 1;
            for (int v = 0; v < 9; v++) {
                if (s[i]->possible[v] != 0) {
                    if (!first) printf(",");
                    printf("%d", v + 1);
                    first = 0;
                }
            }
        }
        if (i < 8) printf(" | ");
    }
    printf("\n");
}

static int countPossible(SudokuTiles *t) {
    int count = 0;
    for (int i = 0; i < 9; i++) {
        count += t->possible[i];
    }
    return count;
}

static int samePossible(SudokuTiles *a, SudokuTiles *b) {
    for (int i = 0; i < 9; i++) {
        if (a->possible[i] != b->possible[i]) return 0;
    }
    return 1;
}

char solveNakedPairInSubset(Subset s) {
    int modified = 0;
    for (int i = 0; i < 9; i++) {

        // Tile already solved
        if (s[i]->value != 0) continue;
        // Not a pair
        if (countPossible(s[i]) != 2) continue;

        // Iterate for the second value of the pair
        for (int j = i + 1; j < 9; j++) {
            // Tile already solved
            if (s[j]->value != 0) continue;
            // Not a pair
            if (countPossible(s[j]) != 2) continue;
            // Not the same possible as the first
            if (!samePossible(s[i], s[j])) continue;

            // We want to iterate every tile to make the pair's candidates impossible in the others
            for (int k = 0; k < 9; k++) {

                // Don't touch the pair
                if (k == i ||k == j) continue;
                // Don't touch the solved ones
                if (s[k]->value != 0 ) continue;

                for (int v = 0; v < 9; v++) {
                    if (s[i]->possible[v]) {
                        if (s[k]->possible[v]) {
                            s[k]->possible[v] = 0;
                            modified = 1;
                        }
                    }
                }
            }
        }
    }
    return (char)modified;
}

char cleanNakedPairs(void) {
    int modified = 0;
    for (int i = 0; i < 27; i++) {
        modified |= solveNakedPairInSubset(all_subset[i]);
    }
    return (char)modified;
}