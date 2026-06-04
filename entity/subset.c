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

static char cleanNakedPairsInSubset(Subset s) {
    int modified = 0;
    for (int i = 0; i < 9; i++) {
        if (s[i]->value != 0 || countPossible(s[i]) != 2) continue;
        for (int j = i+1; j < 9; j++) {
            if (s[j]->value != 0 || countPossible(s[j]) != 2) continue;
            // Même paire
            int same = 1;
            for (int v = 0; v < 9; v++)
                if (s[i]->possible[v] != s[j]->possible[v]) { same = 0; break; }
            if (!same) continue;
            // Éliminer ces 2 valeurs des autres cases
            for (int k = 0; k < 9; k++) {
                if (k == i || k == j || s[k]->value != 0) continue;
                for (int v = 0; v < 9; v++) {
                    if (s[i]->possible[v] != 0 && s[k]->possible[v] != 0) {
                        s[k]->possible[v] = 0;
                        modified = 1;
                    }
                }
            }
        }
    }
    return (char)modified;
}

static char cleanHiddenPairsInSubset(Subset s) {
    int modified = 0;
    for (int v1 = 0; v1 < 9; v1++) {
        for (int v2 = v1+1; v2 < 9; v2++) {
            int pos[9], count = 0;
            for (int i = 0; i < 9; i++) {
                if (s[i]->value != 0) continue;
                if (s[i]->possible[v1] || s[i]->possible[v2])
                    pos[count++] = i;
            }
            if (count != 2) continue;
            int a = pos[0], b = pos[1];
            if (!s[a]->possible[v1] || !s[a]->possible[v2]) continue;
            if (!s[b]->possible[v1] || !s[b]->possible[v2]) continue;
            for (int v = 0; v < 9; v++) {
                if (v == v1 || v == v2) continue;
                if (s[a]->possible[v]) { s[a]->possible[v] = 0; modified = 1; }
                if (s[b]->possible[v]) { s[b]->possible[v] = 0; modified = 1; }
            }
        }
    }
    return (char)modified;
}

static char cleanNakedTriplesInSubset(Subset s) {
    int modified = 0;
    for (int i = 0; i < 9; i++) {
        if (s[i]->value != 0) continue;
        for (int j = i+1; j < 9; j++) {
            if (s[j]->value != 0) continue;
            for (int k = j+1; k < 9; k++) {
                if (s[k]->value != 0) continue;
                int united[9] = {0}, count = 0;
                for (int v = 0; v < 9; v++) {
                    if (s[i]->possible[v] || s[j]->possible[v] || s[k]->possible[v]) {
                        united[v] = 1; count++;
                    }
                }
                if (count != 3) continue;
                for (int m = 0; m < 9; m++) {
                    if (m == i || m == j || m == k || s[m]->value != 0) continue;
                    for (int v = 0; v < 9; v++) {
                        if (united[v] && s[m]->possible[v]) {
                            s[m]->possible[v] = 0; modified = 1;
                        }
                    }
                }
            }
        }
    }
    return (char)modified;
}

static char cleanHiddenTriplesInSubset(Subset s) {
    int modified = 0;
    for (int v1 = 0; v1 < 9; v1++) {
        for (int v2 = v1+1; v2 < 9; v2++) {
            for (int v3 = v2+1; v3 < 9; v3++) {
                int pos[9], count = 0;
                for (int i = 0; i < 9; i++) {
                    if (s[i]->value != 0) continue;
                    if (s[i]->possible[v1] || s[i]->possible[v2] || s[i]->possible[v3])
                        pos[count++] = i;
                }
                if (count != 3) continue;
                int a = pos[0], b = pos[1], c = pos[2];
                if (!(s[a]->possible[v1]||s[b]->possible[v1]||s[c]->possible[v1])) continue;
                if (!(s[a]->possible[v2]||s[b]->possible[v2]||s[c]->possible[v2])) continue;
                if (!(s[a]->possible[v3]||s[b]->possible[v3]||s[c]->possible[v3])) continue;
                for (int v = 0; v < 9; v++) {
                    if (v == v1 || v == v2 || v == v3) continue;
                    if (s[a]->possible[v]) { s[a]->possible[v] = 0; modified = 1; }
                    if (s[b]->possible[v]) { s[b]->possible[v] = 0; modified = 1; }
                    if (s[c]->possible[v]) { s[c]->possible[v] = 0; modified = 1; }
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

