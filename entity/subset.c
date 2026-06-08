#include "subset.h"
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>

// tableau global contenant les 27 sous-ensembles de la grille
Subset all_subset[27];

// retourne un sous-ensemble contenant les 9 cases de la ligne n
Subset getLineSubset(int n) {
    if (n < 1) n = 1;
    if (n > 9) n = 9;
    Subset s = malloc(sizeof(SudokuTiles *) * 9);
    for (int i = 0; i < 9; i++)
        // chaque case de la ligne est à l'indice (n-1)*9 + i dans le tableau
        s[i] = &grid[(n-1)*9 + i];
    return s;
}

// retourne un sous-ensemble contenant les 9 cases de la colonne n
Subset getColSubset(int n) {
    if (n < 1) n = 1;
    if (n > 9) n = 9;
    Subset s = malloc(sizeof(SudokuTiles *) * 9);
    for (int i = 0; i < 9; i++)
        // chaque case de la colonne est espacée de 9 cases dans le tableau
        s[i] = &grid[i*9 + (n-1)];
    return s;
}

/*
 * numérotation des sous-carrés :
 *   1 | 2 | 3
 *   4 | 5 | 6
 *   7 | 8 | 9
 */
// retourne un sous-ensemble contenant les 9 cases du sous-carré n
Subset getSubsqSubset(int n) {
    if (n < 1) n = 1;
    if (n > 9) n = 9;
    Subset s = malloc(sizeof(SudokuTiles *) * 9);
    // calcule la ligne et la colonne du coin haut-gauche du sous-carré
    int start_row = ((n-1) / 3) * 3;
    int start_col = ((n-1) % 3) * 3;
    for (int i = 0; i < 9; i++)
        // parcourt les 3x3 cases du sous-carré avec i/3 pour la ligne et i%3 pour la colonne
        s[i] = &grid[(start_row + i/3)*9 + (start_col + i%3)];
    return s;
}

// construit les 27 sous-ensembles et les stocke dans all_subset
// [0 à 8] = lignes, [9 à 17] = colonnes, [18 à 26] = sous-carrés
void buildAllSubsets(void) {
    for (int i = 0; i < 9; i++) {
        all_subset[i]      = getLineSubset(i + 1);
        all_subset[9  + i] = getColSubset(i + 1);
        all_subset[18 + i] = getSubsqSubset(i + 1);
    }
}

// libère la mémoire allouée pour les 27 sous-ensembles
void freeAllSubsets(void) {
    for (int i = 0; i < 27; i++) {
        free(all_subset[i]);
        all_subset[i] = NULL;
    }
}

// affiche un sous-ensemble case par case
// une case connue affiche sa valeur, une case inconnue affiche ses valeurs encore possibles entre crochets
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

// pour chaque case déjà connue dans le sous-ensemble, retire sa valeur
// des possibilités des autres cases inconnues du même sous-ensemble
// retourne 1 si au moins une modification a eu lieu
char cleanSubset(Subset s) {
    int modified = 0;
    for (int i = 0; i < 9; i++) {
        if (s[i]->value == 0) continue;
        // index 0-based de la valeur connue
        int val = s[i]->value - 1;
        for (int j = 0; j < 9; j++) {
            if (i == j) continue;
            if (s[j]->value != 0) continue;
            // retire la valeur des possibilités de la case voisine
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

// si une valeur n'est possible que dans une seule case inconnue du sous-ensemble,
// alors cette case prend cette valeur (hidden single)
// retourne 1 si au moins une valeur a été fixée
char solveHiddenSinglesInSubset(Subset s) {
    int modified = 0;
    for (int v = 1; v <= 9; v++) {
        // compte combien de cases peuvent encore accueillir la valeur v
        int count = 0, last = -1;
        for (int i = 0; i < 9; i++) {
            if (s[i]->value != 0) continue;
            if (s[i]->possible[v-1] != 0) { count++; last = i; }
        }
        // si une seule case possible, on fixe la valeur
        if (count == 1) {
            int row = (int)(s[last] - grid) / 9 + 1;
            int col = (int)(s[last] - grid) % 9 + 1;
            setTileValue(s[last], (char)v, 0);
            modified = 1;
        }
    }
    return (char)modified;
}

// applique cleanSubset sur les 27 sous-ensembles
// retourne 1 si au moins une modification a eu lieu
char cleanGrid(void) {
    int modified = 0;
    for (int i = 0; i < 27; i++)
        modified |= cleanSubset(all_subset[i]);
    return (char)modified;
}

// applique solveHiddenSinglesInSubset sur les 27 sous-ensembles
// retourne 1 si au moins une valeur a été fixée
char solveHiddenSingles(void) {
    int modified = 0;
    for (int i = 0; i < 27; i++)
        modified |= solveHiddenSinglesInSubset(all_subset[i]);
    return (char)modified;
}

// compte le nombre de valeurs encore possibles dans une case
static int countPossible(SudokuTiles *t) {
    int n = 0;
    for (int v = 0; v < 9; v++)
        if (t->possible[v] != 0) n++;
    return n;
}

// convertit le tableau possible[] d'une case en un masque de bits
// le bit i vaut 1 si la valeur i+1 est encore possible
static int possibleToMask(SudokuTiles *t) {
    int mask = 0;
    for (int v = 0; v < 9; v++)
        if (t->possible[v] != 0) mask |= (1 << v);
    return mask;
}

// retourne le nombre de bits à 1 dans un masque
static int bitCount(int mask) {
    return __builtin_popcount(mask);
}

// réécrit le tableau possible[] d'une case à partir d'un masque de bits
static void maskToPossible(SudokuTiles *t, int mask) {
    for (int i = 0; i < 9; i++) {
        t->possible[i] = (mask >> i) & 1;
    }
}

// si deux cases inconnues d'un sous-ensemble ont exactement les mêmes 2 valeurs possibles,
// ces 2 valeurs sont forcément dans ces cases et peuvent être retirées de toutes les autres
static char cleanNakedPairsInSubset(Subset s) {
    int modified = 0;
    for (int i = 0; i < 9; i++) {
        if (s[i]->value != 0) continue;
        int maskI = possibleToMask(s[i]);
        // on cherche des cases avec exactement 2 possibilités
        if (bitCount(maskI) != 2) continue;
        for (int j = i+1; j < 9; j++) {
            if (s[j]->value != 0) continue;
            int maskJ = possibleToMask(s[j]);
            if (bitCount(maskJ) != 2) continue;
            // paire nue trouvée si les deux cases ont exactement les mêmes possibilités
            if (maskI != maskJ) continue;
            // retire ces 2 valeurs des autres cases du sous-ensemble
            for (int k = 0; k < 9; k++) {
                if (k == i || k == j) continue;
                if (s[k]->value != 0) continue;
                int before = possibleToMask(s[k]);
                // supprime les bits communs avec la paire
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

// retourne un masque de bits indiquant dans quelles cases de s la valeur v est possible
// le bit i vaut 1 si la case i peut contenir v
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

// si deux valeurs ne peuvent apparaître que dans les mêmes 2 cases d'un sous-ensemble,
// toutes les autres valeurs possibles de ces 2 cases sont retirées
static char cleanHiddenPairsInSubset(Subset s) {
    int modified = 0;
    for (int a = 1; a <= 9; a++) {
        int posA = valuePositions(s, a);
        for (int b = a + 1; b <= 9; b++) {
            int posB = valuePositions(s, b);
            // cases communes où a et b sont tous les deux possibles
            int common = posA & posB;
            // paire cachée trouvée si a et b sont contraints à exactement 2 cases
            if (bitCount(common) != 2) continue;
            // garde uniquement a et b dans ces 2 cases, retire le reste
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

// si trois cases inconnues d'un sous-ensemble ont leurs valeurs possibles
// toutes contenues dans un ensemble de 3 valeurs,
// ces 3 valeurs peuvent être retirées de toutes les autres cases
static char cleanNakedTriplesInSubset(Subset s) {
    int modified = 0;
    for (int i = 0; i < 9; i++) {
        if (s[i]->value != 0) continue;
        int maskI = possibleToMask(s[i]);
        // on ne considère que les cases avec au plus 3 possibilités
        if (bitCount(maskI) != 3) continue;
        for (int j = i + 1; j < 9; j++) {
            if (s[j]->value != 0) continue;
            int maskJ = possibleToMask(s[j]);
            // union des possibilités des cases i et j
            int maskIJ = maskI | maskJ;
            if (bitCount(maskIJ) != 3) continue;
            for (int k = j + 1; k < 9; k++) {
                if (s[k]->value != 0) continue;
                int mk = possibleToMask(s[k]);
                // union des possibilités des 3 cases
                int unionMask = maskIJ | mk;
                // triplet nu trouvé si l'union fait exactement 3 valeurs
                if (bitCount(unionMask) != 3) continue;
                int tripletMask = unionMask;
                // retire ces 3 valeurs des autres cases du sous-ensemble
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

// si trois valeurs ne peuvent apparaître que dans les mêmes 3 cases d'un sous-ensemble,
// toutes les autres valeurs possibles de ces 3 cases sont retirées
static char cleanHiddenTriplesInSubset(Subset s) {
    int modified = 0;
    for (int a = 1; a <= 9; a++) {
        int posA = valuePositions(s, a);
        for (int b = a + 1; b <= 9; b++) {
            int posB = valuePositions(s, b);
            for (int c = b + 1; c <= 9; c++) {
                int posC = valuePositions(s, c);
                // union des cases où a, b ou c sont possibles
                int unionPos = posA | posB | posC;
                // triplet caché trouvé si a, b et c sont contraints à exactement 3 cases
                if (bitCount(unionPos) != 3) continue;
                // garde uniquement a, b et c dans ces 3 cases, retire le reste
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

// applique cleanNakedPairsInSubset sur les 27 sous-ensembles
char cleanNakedPairs(void) {
    int m = 0;
    for (int i = 0; i < 27; i++) m |= cleanNakedPairsInSubset(all_subset[i]);
    return (char)m;
}

// applique cleanHiddenPairsInSubset sur les 27 sous-ensembles
char cleanHiddenPairs(void) {
    int m = 0;
    for (int i = 0; i < 27; i++) m |= cleanHiddenPairsInSubset(all_subset[i]);
    return (char)m;
}

// applique cleanNakedTriplesInSubset sur les 27 sous-ensembles
char cleanNakedTriples(void) {
    int m = 0;
    for (int i = 0; i < 27; i++) m |= cleanNakedTriplesInSubset(all_subset[i]);
    return (char)m;
}

// applique cleanHiddenTriplesInSubset sur les 27 sous-ensembles
char cleanHiddenTriples(void) {
    int m = 0;
    for (int i = 0; i < 27; i++) m |= cleanHiddenTriplesInSubset(all_subset[i]);
    return (char)m;
}