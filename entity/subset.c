#include "subset.h"
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>

Subset all_subset[27];

Subset getLineSubset(int n) {
    if (n < 1) n = 1;
    if (n > 9) n = 9;
    Subset s = malloc(sizeof(SudokuTiles *) * 9);
    for (int i = 0; i < 9; i++)
        s[i] = &grid[(n-1)*9 + i];
    return s;
}

Subset getColSubset(int n) {
    if (n < 1) n = 1;
    if (n > 9) n = 9;
    Subset s = malloc(sizeof(SudokuTiles *) * 9);
    for (int i = 0; i < 9; i++)
        s[i] = &grid[i*9 + (n-1)];
    return s;
}

/*
 * Numérotation :
 *   1 | 2 | 3
 *   4 | 5 | 6
 *   7 | 8 | 9
 */
Subset getSubsqSubset(int n) {
    if (n < 1) n = 1;
    if (n > 9) n = 9;
    Subset s = malloc(sizeof(SudokuTiles *) * 9);
    int start_row = ((n-1) / 3) * 3;
    int start_col = ((n-1) % 3) * 3;
    for (int i = 0; i < 9; i++)
        s[i] = &grid[(start_row + i/3)*9 + (start_col + i%3)];
    return s;
}

/*
 * Stocke 27 subsets dans all_subset[27] :
 *   [0 à 8]   lignes 1 à 9
 *   [9 à 17]  colonnes 1 à 9
 *   [18 à 26] sous-carrés 1 à 9
 * Type choisi : tableau plat de taille fixe — suffisant, pas besoin
 * d'allocation dynamique pour le conteneur.
 */
void buildAllSubsets(void) {
    for (int i = 0; i < 9; i++) {
        all_subset[i]      = getLineSubset(i + 1);
        all_subset[9  + i] = getColSubset(i + 1);
        all_subset[18 + i] = getSubsqSubset(i + 1);
    }
}

void freeAllSubsets(void) {
    for (int i = 0; i < 27; i++) {
        free(all_subset[i]);
        all_subset[i] = NULL;
    }
}


/*
 * Format : valeur connue affichée directement,
 * case inconnue affichée comme [1,2,3,...] (valeurs encore possibles).
 */
void dispSubset(Subset s) {
    for (int i = 0; i < 9; i++) {
        if (s[i]->value != 0) {
            printf("%d", s[i]->value);
        } else {
            printf("[");
            int first = 1;
            for (int v = 0; v < 9; v++) {
                if (s[i]->possible[v] != 0) {
                    if (!first) printf(",");
                    printf("%d", v + 1);
                    first = 0;
                }
            }
            printf("]");
        }
        if (i < 8) printf(" | ");
    }
    printf("\n");
}

char cleanSubset(Subset s) {
    int modified = 0;
    for (int i = 0; i < 9; i++) {
        if (s[i]->value == 0) continue;
        int val = s[i]->value - 1;
        for (int j = 0; j < 9; j++) {
            if (i == j) continue;
            if (s[j]->value != 0) continue;
            if (s[j]->possible[val] != 0) {
                int row = (int)(s[j] - grid) / 9 + 1;
                int col = (int)(s[j] - grid) % 9 + 1;
                int rowI = (int)(s[i] - grid) / 9 + 1;
                int colI = (int)(s[i] - grid) % 9 + 1;
                s[j]->possible[val] = 0;
                modified = 1;
            }
        }
    }
    return (char)modified;
}

char solveHiddenSinglesInSubset(Subset s) {
    int modified = 0;
    for (int v = 1; v <= 9; v++) {
        int count = 0, last = -1;
        for (int i = 0; i < 9; i++) {
            if (s[i]->value != 0) continue;
            if (s[i]->possible[v-1] != 0) { count++; last = i; }
        }
        if (count == 1) {
            int row = (int)(s[last] - grid) / 9 + 1;
            int col = (int)(s[last] - grid) % 9 + 1;
            setTileValue(s[last], (char)v, 0);
            modified = 1;
        }
    }
    return (char)modified;
}

char cleanGrid(void) {
    int modified = 0;
    for (int i = 0; i < 27; i++)
        modified |= cleanSubset(all_subset[i]);
    return (char)modified;
}

char solveHiddenSingles(void) {
    int modified = 0;
    for (int i = 0; i < 27; i++)
        modified |= solveHiddenSinglesInSubset(all_subset[i]);
    return (char)modified;
}

static int countPossible(SudokuTiles *t) {
    int n = 0;
    for (int v = 0; v < 9; v++)
        if (t->possible[v] != 0) n++;
    return n;
}

static int possibleToMask(SudokuTiles *t) {
    int mask = 0;
    for (int v = 0; v < 9; v++)
        if (t->possible[v] != 0) mask |= (1 << v);
    return mask;
}

static int bitCount(int mask) {
    return __builtin_popcount(mask);
}

static void maskToPossible(SudokuTiles *t, int mask) {
    for (int i = 0; i < 9; i++) {
        t->possible[i] = (mask >> i) & 1;
    }
}

static char cleanNakedPairsInSubset(Subset s) {
    int modified = 0;
    for (int i = 0; i < 9; i++) {

        if (s[i]->value != 0 ) continue;
        int maskI = possibleToMask(s[i]);
        if (bitCount(maskI) != 2) continue;
        for (int j = i+1; j < 9; j++) {
            if (s[j]->value != 0) continue;
            int maskJ = possibleToMask(s[j]);
            if (bitCount(maskJ) != 2) continue;
            // Paire possible
            if (maskI != maskJ) continue;
            // Éliminer ces 2 valeurs des autres cases
            for (int k = 0; k < 9; k++) {
                if (k == i || k == j) continue;
                if (s[k]->value != 0) continue;

                int before = possibleToMask(s[k]);
                int after = before & ~maskI;

                if (before != after) {
                    modified = 1;
                    maskToPossible(s[k], after);
                }
            }
        }
    }
    return (char)modified;
}

static int valuePositions(Subset s, int v) {
    int pos = 0;

    for (int i = 0; i < 9; i++) {
        if (s[i]->value != 0) continue;

        int mask = possibleToMask(s[i]);
        if (mask & (1 << (v - 1))) {
            pos |= (1 << i);
        }
    }

    return pos;
}

static char cleanHiddenPairsInSubset(Subset s) {
    int modified = 0;

    for (int a = 1; a <= 9; a++) {
        int posA = valuePositions(s, a);

        for (int b = a + 1; b <= 9; b++) {
            int posB = valuePositions(s, b);

            int common = posA & posB;

            if (bitCount(common) != 2) continue;

            // hidden pair found in exactly 2 cells

            int pairMask = (1 << (a - 1)) | (1 << (b - 1));

            for (int i = 0; i < 9; i++) {
                if (s[i]->value != 0) continue;
                if (!(common & (1 << i))) continue;


                int before = possibleToMask(s[i]);
                int after  = before & pairMask;

                if (before != after) {
                    maskToPossible(s[i], after);
                    modified = 1;
                }
            }
        }
    }
    return (char)modified;
}

static char cleanNakedTriplesInSubset(Subset s) {
    int modified = 0;

    for (int i = 0; i < 9; i++) {
        if (s[i]->value != 0) continue;

        int maskI = possibleToMask(s[i]);
        if (bitCount(maskI) != 3) continue;

        for (int j = i + 1; j < 9; j++) {
            if (s[j]->value != 0) continue;

            int maskJ = possibleToMask(s[j]);
            int maskIJ = maskI | maskJ;

            if (bitCount(maskIJ) != 3) continue;

            for (int k = j + 1; k < 9; k++) {
                if (s[k]->value != 0) continue;

                int mk = possibleToMask(s[k]);
                int unionMask = maskIJ | mk;

                if (bitCount(unionMask) != 3) continue;

                int tripletMask = unionMask;

                for (int x = 0; x < 9; x++) {
                    if (x == i || x == j || x == k) continue;
                    if (s[x]->value != 0) continue;

                    int before = possibleToMask(s[x]);
                    int after  = before & ~tripletMask;

                    if (before != after) {
                        maskToPossible(s[x], after);
                        modified = 1;
                    }
                }
            }
        }
    }

    return (char)modified;
}

static char cleanHiddenTriplesInSubset(Subset s) {
    int modified = 0;

    for (int a = 1; a <= 9; a++) {
        int posA = valuePositions(s, a);

        for (int b = a + 1; b <= 9; b++) {
            int posB = valuePositions(s, b);

            for (int c = b + 1; c <= 9; c++) {
                int posC = valuePositions(s, c);

                int unionPos = posA | posB | posC;

                if (bitCount(unionPos) != 3) continue;

                // hidden triplet found in exactly 3 cells

                int tripletMask =
                    (1 << (a - 1)) |
                    (1 << (b - 1)) |
                    (1 << (c - 1));

                for (int i = 0; i < 9; i++) {

                    if (s[i]->value != 0) continue;
                    if (!(unionPos & (1 << i))) continue;

                    int before = possibleToMask(s[i]);
                    int after  = before & tripletMask;

                    if (before != after) {
                        maskToPossible(s[i], after);
                        modified = 1;
                    }
                }
            }
        }
    }

    return (char)modified;
}

char cleanNakedPairs(void) {
    int m = 0;
    for (int i = 0; i < 27; i++) m |= cleanNakedPairsInSubset(all_subset[i]);
    return (char)m;
}
char cleanHiddenPairs(void) {
    int m = 0;
    for (int i = 0; i < 27; i++) m |= cleanHiddenPairsInSubset(all_subset[i]);
    return (char)m;
}
char cleanNakedTriples(void) {
    int m = 0;
    for (int i = 0; i < 27; i++) m |= cleanNakedTriplesInSubset(all_subset[i]);
    return (char)m;
}
char cleanHiddenTriples(void) {
    int m = 0;
    for (int i = 0; i < 27; i++) m |= cleanHiddenTriplesInSubset(all_subset[i]);
    return (char)m;
}

