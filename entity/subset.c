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

// applique solveHiddenSinglesInSubset sur les 27 sous-ensembles
// retourne 1 si au moins une valeur a été fixée
char solveHiddenSingles(void) {
    int modified = 0;
    for (int i = 0; i < 27; i++)
        modified |= solveHiddenSinglesInSubset(all_subset[i]);
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

// retourne le numero de ligne 1 à 9 de la case k (0-based dans grid)
static int rowOf(int k) { return k / 9 + 1; }

//retourne le numero de colonne 1 à 9 de la case k (0-based dans grid)
static int colOf(int k) { return k % 9 + 1; }

// retourne le numero de sous-carre 1 à 9 de la case k (0-based dans grid)
static int subsqOf(int k) {
    int r = k / 9;
    int c = k % 9;
    return (r/3)*3 + (c/3) + 1;
}

/**
 * pointing pairs: pour chaque subsquare, si une valeur n'est
 * possible que sur une seule ligne ou col. à l'interieur du
 * subsquare, on retire cette valeur des autres cases de cette
 * ligne ou col hors du subsquare.
 **/
char cleanPointingPairs(void) {
    int modified = 0;

    for (int sq = 1; sq <= 9; sq++) {
        Subset s = getSubsqSubset(sq);

        for (int v = 1; v <= 9; v++) {
            // marque les lignes et colonnes du sous-carre ou v est encore possible
            char row_seen[10] = {0}; // index 1 à 9
            char col_seen[10] = {0};
            int  distinct_rows = 0;
            int  distinct_cols = 0;
            int  last_row = 0;
            int  last_col = 0;
            int  count = 0;

            for (int i = 0; i < 9; i++) {
                if (s[i]->value != 0) continue;
                if (s[i]->possible[v-1] == 0) continue;

                int idx = (int)(s[i] - grid);
                int row = rowOf(idx);
                int col = colOf(idx);

                if (!row_seen[row]) {
                    row_seen[row] = 1;
                    distinct_rows++;
                    last_row = row;
                }
                if (!col_seen[col]) {
                    col_seen[col] = 1;
                    distinct_cols++;
                    last_col = col;
                }
                count++;
            }

            if (count == 0) continue;

            // v confine a une seule ligne et on la retire hors du sous-carre
            if (distinct_rows == 1) {
                Subset line = getLineSubset(last_row);
                for (int j = 0; j < 9; j++) {
                    if (line[j]->value != 0) continue;
                    if (subsqOf((int)(line[j] - grid)) == sq) continue; /* dans le sous-carre : skip */
                    if (line[j]->possible[v-1] != 0) {
                        line[j]->possible[v-1] = 0;
                        modified = 1;
                    }
                }
                free(line);
            }

            // v réduit a une seule colonne -> on la retire hors du sous-carre
            if (distinct_cols == 1) {
                Subset column = getColSubset(last_col);
                for (int j = 0; j < 9; j++) {
                    if (column[j]->value != 0) continue;
                    if (subsqOf((int)(column[j] - grid)) == sq) continue;
                    if (column[j]->possible[v-1] != 0) {
                        column[j]->possible[v-1] = 0;
                        modified = 1;
                    }
                }
                free(column);
            }
        }
        free(s);
    }
    return (char)modified;
}

/**
 * box-line reduction: pour chaque ligne et chaque colonne, si une
 * valeur n'est possible que dans un seul subsquare à l'interieur de
 * cette ligne (ou col), on retire cette valeur des autres cases
 * de ce subsquare hors de la ligne (ou colonne).
 **/
char cleanBoxLineReduction(void) {
    int modified = 0;

    // lignes
    for (int row = 1; row <= 9; row++) {
        Subset line = getLineSubset(row);

        for (int v = 1; v <= 9; v++) {
            char sq_seen[10] = {0}; // index 1 à 9
            int  distinct_sq = 0;
            int  last_sq = 0;
            int  count = 0;
            for (int i = 0; i < 9; i++) {
                if (line[i]->value != 0) continue;
                if (line[i]->possible[v-1] == 0) continue;

                int idx = (int)(line[i] - grid);
                int sq  = subsqOf(idx);

                if (!sq_seen[sq]) {
                    sq_seen[sq] = 1;
                    distinct_sq++;
                    last_sq = sq;
                }
                count++;
            }

            if (count == 0) continue;

            if (distinct_sq == 1) {
                Subset square = getSubsqSubset(last_sq);
                for (int j = 0; j < 9; j++) {
                    if (square[j]->value != 0) continue;
                    if (rowOf((int)(square[j] - grid)) == row) continue; // dans la ligne : skip
                    if (square[j]->possible[v-1] != 0) {
                        square[j]->possible[v-1] = 0;
                        modified = 1;
                    }
                }
                free(square);
            }
        }

        free(line);
    }

    // colonnes
    for (int col = 1; col <= 9; col++) {
        Subset column = getColSubset(col);
        for (int v = 1; v <= 9; v++) {
            char sq_seen[10] = {0};
            int  distinct_sq = 0;
            int  last_sq = 0;
            int  count = 0;
            for (int i = 0; i < 9; i++) {
                if (column[i]->value != 0) continue;
                if (column[i]->possible[v-1] == 0) continue;

                int idx = (int)(column[i] - grid);
                int sq  = subsqOf(idx);

                if (!sq_seen[sq]) {
                    sq_seen[sq] = 1;
                    distinct_sq++;
                    last_sq = sq;
                }
                count++;
            }

            if (count == 0) continue;
            if (distinct_sq == 1) {
                Subset square = getSubsqSubset(last_sq);
                for (int j = 0; j < 9; j++) {
                    if (square[j]->value != 0) continue;
                    if (colOf((int)(square[j] - grid)) == col) continue;
                    if (square[j]->possible[v-1] != 0) {
                        square[j]->possible[v-1] = 0;
                        modified = 1;
                    }
                }
                free(square);
            }
        }
        free(column);
    }
    return (char)modified;
}

/**
 * Y-Wing
 *
 * Un Y-Wing n'implique que 3 cases :
 *   -pivot  : case avec exactement 2 valeurs possibles {a, b}
 *   -pince1 : case visible par le pivot avec exactement 2 valeurs {a, c}
 *   -pince2 : case visible par le pivot avec exactement 2 valeurs {b, c}
 *
 * visible signifie : meme ligne, meme colonne, ou meme sous-carre.
 *
 * Conclusion : toute case visible a la fois par pince1 ET pince2
 * ne peut pas valoir c.
 **/

// retourne le numero de ligne (1 à 9) de la case k
static int ywRowOf(int k) {
    return k / 9 + 1;
}

// retourne le numero de colonne (1 à 9) de la case k
static int ywColOf(int k) {
    return k % 9 + 1;
}

// retourne le numero du subsquare (1 à 9) de la case k
static int ywSubsqOf(int k) {
    int r = k / 9;
    int c = k % 9;
    return (r/3)*3 + (c/3) + 1;
}

/* retourne 1 si les cases aux indices idx1 et idx2 sont "visibles"
 * l'une de l'autre (meme ligne, colonne ou sous-carre) */
static int visible(int idx1, int idx2) {
    if (idx1 == idx2) return 0;
    if (ywRowOf(idx1) == ywRowOf(idx2)) return 1;
    if (ywColOf(idx1) == ywColOf(idx2)) return 1;
    if (ywSubsqOf(idx1) == ywSubsqOf(idx2)) return 1;
    return 0;
}

/* compte le nombre de valeurs possibles d'une case */
static int countPossibleYWing(SudokuTiles *t) {
    int n = 0;
    for (int v = 0; v < 9; v++)
        if (t->possible[v]) n++;
    return n;
}

/* remplit vals[0] et vals[1] avec les 2 valeurs possibles d'une case
 * qui en a exactement 2. retourne 0 si la case n'a pas exactement 2
 * valeurs possibles. */
static int getTwoPossibles(SudokuTiles *t, int *v1, int *v2) {
    int found = 0;
    for (int v = 0; v < 9; v++) {
        if (t->possible[v]) {
            if (found == 0) *v1 = v + 1;
            else            *v2 = v + 1;
            found++;
        }
    }
    return found == 2;
}

/*
 * Y-Wing : parcourt toutes les cases comme pivot, cherche deux
 * pinces valides, puis retire la valeur commune c de toutes les
 * cases visibles par les deux pinces.
 * retourne 1 si au moins une modification a eu lieu.
 */
char cleanYWing(void) {
    int modified = 0;

    for (int p = 0; p < GRID_SIZE; p++) {
        // le pivot doit etre une case inconnue avec exactement 2 possibles
        if (grid[p].value != 0) continue;
        if (countPossible(&grid[p]) != 2) continue;

        int pa, pb; // les 2 valeurs du pivot : a, b */
        if (!getTwoPossibles(&grid[p], &pa, &pb)) continue;

        // cherche pince1 : case visible par le pivot avec {a, c}
        for (int w1 = 0; w1 < GRID_SIZE; w1++) {
            if (grid[w1].value != 0) continue;
            if (!visible(p, w1)) continue;
            if (countPossibleYWing(&grid[w1]) != 2) continue;

            int w1a, w1b;
            if (!getTwoPossibles(&grid[w1], &w1a, &w1b)) continue;

            // pince1 doit partager exactement une valeur avec le pivot
            int shared1 = 0; // la valeur partagee avec le pivot
            int c1 = 0;      // la valeur unique de pince1

            if (w1a == pa && w1b != pb) { shared1 = pa; c1 = w1b; }
            else if (w1a == pb && w1b != pa) { shared1 = pb; c1 = w1b; }
            else if (w1b == pa && w1a != pb) { shared1 = pa; c1 = w1a; }
            else if (w1b == pb && w1a != pa) { shared1 = pb; c1 = w1a; }
            else continue; /* pas de partage valide */

            /* la valeur manquante du pivot (celle que pince2 doit partager) */
            int need = (shared1 == pa) ? pb : pa;

            /* cherche pince2 : case visible par le pivot avec {need, c1}
             * et differente de pince1 */
            for (int w2 = 0; w2 < GRID_SIZE; w2++) {
                if (w2 == w1) continue;
                if (grid[w2].value != 0) continue;
                if (!visible(p, w2)) continue;
                if (countPossibleYWing(&grid[w2]) != 2) continue;

                int w2a, w2b;
                if (!getTwoPossibles(&grid[w2], &w2a, &w2b)) continue;

                /* pince2 doit avoir exactement {need, c1} */
                int has_need = (w2a == need || w2b == need);
                int has_c1   = (w2a == c1   || w2b == c1);
                if (!has_need || !has_c1) continue;

                /* pince2 ne doit pas partager la meme valeur partagee
                 * que pince1 avec le pivot (sinon ce n'est plus un Y-Wing) */
                if (w2a == shared1 || w2b == shared1) continue;

                /* Y-Wing valide : retire c1 de toutes les cases visibles
                 * a la fois par pince1 (w1) et pince2 (w2) */
                for (int t2 = 0; t2 < GRID_SIZE; t2++) {
                    if (t2 == p || t2 == w1 || t2 == w2) continue;
                    if (grid[t2].value != 0) continue;
                    if (!visible(w1, t2)) continue;
                    if (!visible(w2, t2)) continue;
                    if (grid[t2].possible[c1 - 1] != 0) {
                        grid[t2].possible[c1 - 1] = 0;
                        modified = 1;
                    }
                }
            }
        }
    }

    return (char)modified;
}

/* retourne le numero de ligne (1..9) de la case k (0-based dans grid) */
static int adv_rowOf(int k) { return k / 9 + 1; }

/* retourne le numero de colonne (1..9) de la case k (0-based dans grid) */
static int adv_colOf(int k) { return k % 9 + 1; }

/* retourne le numero de sous-carre (1..9) de la case k */
static int adv_subsqOf(int k) {
    int r = k / 9;
    int c = k % 9;
    return (r/3)*3 + (c/3) + 1;
}

/*
 * pour une ligne donnee et une valeur v, remplit cols[] avec les
 * numeros de colonnes (1..9) ou v est encore possible, et retourne
 * leur nombre.
 */
static int getColsForValueInRow(int row, int v, int cols[9]) {
    int count = 0;
    for (int col = 1; col <= 9; col++) {
        SudokuTiles *t = &grid[(row-1)*9 + (col-1)];
        if (t->value != 0) continue;
        if (t->possible[v-1] != 0)
            cols[count++] = col;
    }
    return count;
}

/*
 * pour une colonne donnee et une valeur v, remplit rows[] avec les
 * numeros de lignes 1 à 9 où v est encore possible, et retourne
 * leur nombre.
 */
static int getRowsForValueInCol(int col, int v, int rows[9]) {
    int count = 0;
    for (int row = 1; row <= 9; row++) {
        SudokuTiles *t = &grid[(row-1)*9 + (col-1)];
        if (t->value != 0) continue;
        if (t->possible[v-1] != 0)
            rows[count++] = row;
    }
    return count;
}

// retourne 1 si val est dans le tableau arr de taille n
static int contains(int *arr, int n, int val) {
    for (int i = 0; i < n; i++)
        if (arr[i] == val) return 1;
    return 0;
}

// calcule l'union de deux tableaux dans dst[], retourne la taille du tableau
static int unionArrays(int *a, int na, int *b, int nb, int dst[9]) {
    int size = 0;
    for (int i = 0; i < na; i++)
        dst[size++] = a[i];
    for (int i = 0; i < nb; i++)
        if (!contains(dst, size, b[i]))
            dst[size++] = b[i];
    return size;
}

// calcule l'union de trois tableaux dans dst[], retourne la taille
static int unionThree(int *a, int na, int *b, int nb,
                      int *c, int nc, int dst[9]) {
    int tmp[9];
    int ntmp = unionArrays(a, na, b, nb, tmp);
    return unionArrays(tmp, ntmp, c, nc, dst);
}

// calcule l'union de quatre tableaux dans dst[], retourne la taille
static int unionFour(int *a, int na, int *b, int nb,
                     int *c, int nc, int *d, int nd, int dst[9]) {
    int tmp[9];
    int ntmp = unionThree(a, na, b, nb, c, nc, tmp);
    return unionArrays(tmp, ntmp, d, nd, dst);
}

/*
 * X-Wing par lignes :
 * si une valeur v n'apparait que dans exactement 2 cases sur 2 lignes
 * differentes, et que ces cases sont dans les memes 2 colonnes, alors
 * v peut etre retiree de toutes les autres cases de ces 2 colonnes.
 *
 * on fait de meme par colonnes (en echangeant les roles ligne/colonne).
 */
char cleanXWing(void) {
    int modified = 0;

    for (int v = 1; v <= 9; v++) {

        // X Wing par lignes
        for (int r1 = 1; r1 <= 9; r1++) {
            int cols1[9];
            int n1 = getColsForValueInRow(r1, v, cols1);
            if (n1 != 2) continue;

            for (int r2 = r1 + 1; r2 <= 9; r2++) {
                int cols2[9];
                int n2 = getColsForValueInRow(r2, v, cols2);
                if (n2 != 2) continue;

                // les 2 lignes doivent avoir v dans les memes 2 colonnes
                if (cols1[0] != cols2[0] || cols1[1] != cols2[1]) continue;

                // X Wing trouvé: retire v des autres cases de ces 2 colonnes
                for (int c = 0; c < 2; c++) {
                    for (int row = 1; row <= 9; row++) {
                        if (row == r1 || row == r2) continue;
                        SudokuTiles *t = &grid[(row-1)*9 + (cols1[c]-1)];
                        if (t->value != 0) continue;
                        if (t->possible[v-1] != 0) {
                            t->possible[v-1] = 0;
                            modified = 1;
                        }
                    }
                }
            }
        }

        // XWing par colonnes
        for (int c1 = 1; c1 <= 9; c1++) {
            int rows1[9];
            int n1 = getRowsForValueInCol(c1, v, rows1);
            if (n1 != 2) continue;

            for (int c2 = c1 + 1; c2 <= 9; c2++) {
                int rows2[9];
                int n2 = getRowsForValueInCol(c2, v, rows2);
                if (n2 != 2) continue;

                if (rows1[0] != rows2[0] || rows1[1] != rows2[1]) continue;

                for (int r = 0; r < 2; r++) {
                    for (int col = 1; col <= 9; col++) {
                        if (col == c1 || col == c2) continue;
                        SudokuTiles *t = &grid[(rows1[r]-1)*9 + (col-1)];
                        if (t->value != 0) continue;
                        if (t->possible[v-1] != 0) {
                            t->possible[v-1] = 0;
                            modified = 1;
                        }
                    }
                }
            }
        }
    }

    return (char)modified;
}



/**
 * Swordfish par lignes:
 * si une valeur v n'apparait que dans 2 ou 3 cases sur 3 lignes
 * differentes, et que toutes ces cases tiennent dans les memes
 * 3 colonnes, alors v peut etre retiree de toutes les autres cases
 * de ces 3 colonnes.
 */
char cleanSwordfish(void) {
    int modified = 0;

    for (int v = 1; v <= 9; v++) {

        // Swordfish par lignes
        for (int r1 = 1; r1 <= 9; r1++) {
            int cols1[9];
            int n1 = getColsForValueInRow(r1, v, cols1);
            if (n1 < 2 || n1 > 3) continue;

            for (int r2 = r1 + 1; r2 <= 9; r2++) {
                int cols2[9];
                int n2 = getColsForValueInRow(r2, v, cols2);
                if (n2 < 2 || n2 > 3) continue;

                for (int r3 = r2 + 1; r3 <= 9; r3++) {
                    int cols3[9];
                    int n3 = getColsForValueInRow(r3, v, cols3);
                    if (n3 < 2 || n3 > 3) continue;

                    // union des colonnes des 3 lignes: doit être == 3
                    int all_cols[9];
                    int ntotal = unionThree(cols1, n1, cols2, n2,
                                           cols3, n3, all_cols);
                    if (ntotal != 3) continue;

                    // Swordfish trouve : retire v des autres cases de ces 3 colonnes
                    for (int ci = 0; ci < 3; ci++) {
                        for (int row = 1; row <= 9; row++) {
                            if (row == r1 || row == r2 || row == r3) continue;
                            SudokuTiles *t = &grid[(row-1)*9 + (all_cols[ci]-1)];
                            if (t->value != 0) continue;
                            if (t->possible[v-1] != 0) {
                                t->possible[v-1] = 0;
                                modified = 1;
                            }
                        }
                    }
                }
            }
        }

        // Swordfish par colonnes
        for (int c1 = 1; c1 <= 9; c1++) {
            int rows1[9];
            int n1 = getRowsForValueInCol(c1, v, rows1);
            if (n1 < 2 || n1 > 3) continue;

            for (int c2 = c1 + 1; c2 <= 9; c2++) {
                int rows2[9];
                int n2 = getRowsForValueInCol(c2, v, rows2);
                if (n2 < 2 || n2 > 3) continue;

                for (int c3 = c2 + 1; c3 <= 9; c3++) {
                    int rows3[9];
                    int n3 = getRowsForValueInCol(c3, v, rows3);
                    if (n3 < 2 || n3 > 3) continue;

                    int all_rows[9];
                    int ntotal = unionThree(rows1, n1, rows2, n2,
                                           rows3, n3, all_rows);
                    if (ntotal != 3) continue;

                    for (int ri = 0; ri < 3; ri++) {
                        for (int col = 1; col <= 9; col++) {
                            if (col == c1 || col == c2 || col == c3) continue;
                            SudokuTiles *t = &grid[(all_rows[ri]-1)*9 + (col-1)];
                            if (t->value != 0) continue;
                            if (t->possible[v-1] != 0) {
                                t->possible[v-1] = 0;
                                modified = 1;
                            }
                        }
                    }
                }
            }
        }
    }

    return (char)modified;
}

/*
 * Jellyfish par lignes :
 * generalisation du Swordfish a 4 lignes et 4 colonnes.
 * si une valeur v n'apparait que dans 2, 3 ou 4 cases sur 4 lignes
 * differentes, et que toutes ces cases tiennent dans les memes
 * 4 colonnes, alors v peut etre retiree des autres cases de ces
 * 4 colonnes.
 */
char cleanJellyfish(void) {
    int modified = 0;

    for (int v = 1; v <= 9; v++) {

        // par lignes
        for (int r1 = 1; r1 <= 9; r1++) {
            int cols1[9];
            int n1 = getColsForValueInRow(r1, v, cols1);
            if (n1 < 2 || n1 > 4) continue;

            for (int r2 = r1 + 1; r2 <= 9; r2++) {
                int cols2[9];
                int n2 = getColsForValueInRow(r2, v, cols2);
                if (n2 < 2 || n2 > 4) continue;

                for (int r3 = r2 + 1; r3 <= 9; r3++) {
                    int cols3[9];
                    int n3 = getColsForValueInRow(r3, v, cols3);
                    if (n3 < 2 || n3 > 4) continue;

                    for (int r4 = r3 + 1; r4 <= 9; r4++) {
                        int cols4[9];
                        int n4 = getColsForValueInRow(r4, v, cols4);
                        if (n4 < 2 || n4 > 4) continue;

                        int all_cols[9];
                        int ntotal = unionFour(cols1, n1, cols2, n2,
                                              cols3, n3, cols4, n4, all_cols);
                        if (ntotal != 4) continue;

                        for (int ci = 0; ci < 4; ci++) {
                            for (int row = 1; row <= 9; row++) {
                                if (row == r1 || row == r2 ||
                                    row == r3 || row == r4) continue;
                                SudokuTiles *t = &grid[(row-1)*9 + (all_cols[ci]-1)];
                                if (t->value != 0) continue;
                                if (t->possible[v-1] != 0) {
                                    t->possible[v-1] = 0;
                                    modified = 1;
                                }
                            }
                        }
                    }
                }
            }
        }

        // par colonnes
        for (int c1 = 1; c1 <= 9; c1++) {
            int rows1[9];
            int n1 = getRowsForValueInCol(c1, v, rows1);
            if (n1 < 2 || n1 > 4) continue;

            for (int c2 = c1 + 1; c2 <= 9; c2++) {
                int rows2[9];
                int n2 = getRowsForValueInCol(c2, v, rows2);
                if (n2 < 2 || n2 > 4) continue;

                for (int c3 = c2 + 1; c3 <= 9; c3++) {
                    int rows3[9];
                    int n3 = getRowsForValueInCol(c3, v, rows3);
                    if (n3 < 2 || n3 > 4) continue;

                    for (int c4 = c3 + 1; c4 <= 9; c4++) {
                        int rows4[9];
                        int n4 = getRowsForValueInCol(c4, v, rows4);
                        if (n4 < 2 || n4 > 4) continue;

                        int all_rows[9];
                        int ntotal = unionFour(rows1, n1, rows2, n2,
                                              rows3, n3, rows4, n4, all_rows);
                        if (ntotal != 4) continue;

                        for (int ri = 0; ri < 4; ri++) {
                            for (int col = 1; col <= 9; col++) {
                                if (col == c1 || col == c2 ||
                                    col == c3 || col == c4) continue;
                                SudokuTiles *t = &grid[(all_rows[ri]-1)*9 + (col-1)];
                                if (t->value != 0) continue;
                                if (t->possible[v-1] != 0) {
                                    t->possible[v-1] = 0;
                                    modified = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return (char)modified;
}

/*
 * Unique Rectangle (type 1) :
 *
 * un rectangle unique est un pattern de 4 cases formant un rectangle
 * sur 2 lignes et 2 colonnes, toutes dans exactement 2 sous-carres
 * differents, ayant toutes les 2 memes valeurs candidates {a, b}.
 *
 * si 3 des 4 cases n'ont QUE {a, b} comme possibles (cases dites
 * "toits"), et que la 4e case (le "sol") a d'autres possibles en plus,
 * alors a et b peuvent etre retires du sol : si on les gardait, la
 * grille aurait 2 solutions, ce qui est interdit pour un sudoku valide.
 *
 */
char cleanUniqueRectangle(void) {
    int modified = 0;

    // on cherche 4 cases aux coins (r1,c1), (r1,c2), (r2,c1), (r2,c2)
    for (int r1 = 1; r1 <= 9; r1++) {
        for (int r2 = r1 + 1; r2 <= 9; r2++) {
            for (int c1 = 1; c1 <= 9; c1++) {
                for (int c2 = c1 + 1; c2 <= 9; c2++) {

                    int idx[4];
                    idx[0] = (r1-1)*9 + (c1-1);
                    idx[1] = (r1-1)*9 + (c2-1);
                    idx[2] = (r2-1)*9 + (c1-1);
                    idx[3] = (r2-1)*9 + (c2-1);

                    // toutes les cases doivent etre inconnues
                    int all_unknown = 1;
                    for (int i = 0; i < 4; i++)
                        if (grid[idx[i]].value != 0) { all_unknown = 0; break; }
                    if (!all_unknown) continue;

                    /* le rectangle doit couvrir exactement 2 sous-carres
                     * (sinon la contrainte unique rectangle ne s'applique pas) */
                    int sq1 = adv_subsqOf(idx[0]);
                    int sq2 = adv_subsqOf(idx[1]);
                    int sq3 = adv_subsqOf(idx[2]);
                    int sq4 = adv_subsqOf(idx[3]);

                    // on veut exactement 2 sous-carres distincts
                    int sq_a = sq1;
                    int sq_b = -1;
                    for (int i = 1; i < 4; i++) {
                        int sq = (i==1)?sq2:(i==2)?sq3:sq4;
                        if (sq != sq_a) {
                            if (sq_b == -1) sq_b = sq;
                            else if (sq != sq_b) { sq_b = -2; break; }
                        }
                    }
                    if (sq_b <= 0) continue; // pas exactement 2 sous-carres

                    // cherche 2 valeurs communes a toutes les cases
                    int common_a = 0, common_b = 0;
                    int found = 0;
                    for (int v = 0; v < 9; v++) {
                        int in_all = 1;
                        for (int i = 0; i < 4; i++)
                            if (!grid[idx[i]].possible[v]) { in_all = 0; break; }
                        if (in_all) {
                            if (found == 0) common_a = v + 1;
                            else if (found == 1) common_b = v + 1;
                            else { found = 99; break; } /* plus de 2 valeurs communes */
                            found++;
                        }
                    }
                    if (found != 2) continue;

                    /* compte le nombre de cases qui ont EXACTEMENT {a, b}
                     * (les "toits") et trouve le "sol" */
                    int nb_roof = 0;
                    int floor_idx = -1;

                    for (int i = 0; i < 4; i++) {
                        int count = 0;
                        for (int v = 0; v < 9; v++)
                            if (grid[idx[i]].possible[v]) count++;

                        if (count == 2) {
                            nb_roof++;
                        } else {
                            floor_idx = idx[i];
                        }
                    }

                    // type 1 : 3 "toits "et 1 "sol "
                    if (nb_roof != 3 || floor_idx == -1) continue;

                    // retire a et b du "sol"
                    if (grid[floor_idx].possible[common_a - 1] != 0) {
                        grid[floor_idx].possible[common_a - 1] = 0;
                        modified = 1;
                    }
                    if (grid[floor_idx].possible[common_b - 1] != 0) {
                        grid[floor_idx].possible[common_b - 1] = 0;
                        modified = 1;
                    }
                }
            }
        }
    }
    return (char)modified;
}