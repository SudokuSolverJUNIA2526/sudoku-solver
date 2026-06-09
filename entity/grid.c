#include "grid.h"
#include <stdio.h>
#include <string.h>

#include "subset.h"

SudokuTiles grid[GRID_SIZE];
Affectation history[HISTORY_SIZE];
int         history_index = 0;

// remet la grille à zéro : toutes les cases sont inconnues et toutes les valeurs sont possibles
// réinitialise aussi l'historique des affectations
void initGrid(void) {
    for (int k = 0; k < GRID_SIZE; k++) {
        grid[k].value = 0;
        // toutes les valeurs 1 à 9 sont marquées comme possibles
        for (int d = 0; d < 9; d++)
            grid[k].possible[d] = 1;
    }
    // remet le curseur de l'historique au début
    history_index = 0;
    memset(history, 0, sizeof(history));
}

// affiche une ligne de séparation horizontale entre les sous-carrés
static void printHSep(void) {
    printf("+-------+-------+-------+\n");
}

// affiche la grille avec uniquement les valeurs certaines
// les cases inconnues apparaissent comme des espaces vides
// les séparations entre sous-carrés sont clairement visibles
void dispFinal(void) {
    printHSep();
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            // séparateur vertical au début de chaque groupe de 3 colonnes
            if (j == 1 || j == 4 || j == 7) printf("| ");
            SudokuTiles *t = &grid[(i-1)*9 + (j-1)];
            if (t->value != 0) printf("%c ", '0' + t->value);
            else               printf("  ");
        }
        printf("|\n");
        // séparateur horizontal après chaque groupe de 3 lignes
        if (i == 3 || i == 6 || i == 9) printHSep();
    }
}

// affiche 9 grilles superposées, une par chiffre de 1 à 9
// pour chaque case :
//   - la valeur si elle est connue et vaut d
//   - un point si la case est connue mais vaut autre chose
//   - un + si la case est inconnue et d est encore possible
//   - un espace si la case est inconnue et d est impossible
void dispPossible(void) {
    for (int d = 1; d <= 9; d++) {
        printf("Possibilities for: %d\n", d);
        printHSep();
        for (int i = 1; i <= 9; i++) {
            for (int j = 1; j <= 9; j++) {
                if (j == 1 || j == 4 || j == 7) printf("| ");
                SudokuTiles *t = &grid[(i-1)*9 + (j-1)];
                if (t->value != 0) {
                    // affiche d si c'est la bonne valeur, sinon un point
                    printf("%c ", t->value == (char)d ? '0'+d : '.');
                } else {
                    // affiche + si d est encore possible, sinon un espace
                    printf("%c ", t->possible[d-1] != 0 ? '+' : ' ');
                }
            }
            printf("|\n");
            if (i == 3 || i == 6 || i == 9) printHSep();
        }
        printf("\n");
    }
}

// fixe la valeur d'une case et met à jour ses possibilités en conséquence
// enregistre l'opération dans l'historique pour pouvoir revenir en arrière
// supposed vaut 1 si c'est une supposition, 0 si c'est une déduction certaine
void setTileValue(SudokuTiles *t, char val, char supposed) {
    t->value = val;
    // retire toutes les possibilités de la case
    for (int d = 0; d < 9; d++) {
        if (t->possible[d] != 0) {
            t->possible[d] = 0;
        }
    }
    // remet uniquement la valeur fixée comme possible
    if (val >= 1 && val <= 9)
        t->possible[val - 1] = 1;

    // enregistre l'affectation dans l'historique tant qu'il reste de la place
    if (history_index < HISTORY_SIZE) {
        history[history_index].tile     = t;
        history[history_index].supposed = supposed;
        history[history_index].value    = val;
        history_index++;
    }
}

char isGridValid(void) {
    int possible_check = 0;
    // check dans les 81 cases de la grille
    for (int i = 0; i < GRID_SIZE; i++) {
        // remise à 0 du compteur de possiblité restante dans une case
        possible_check = 0;
        for (int j = 0; j < 9; j++) {
            // si aucune possiblité pour valeur i et case de la valeur == 0, incrémenter le compteur
            if (grid[i].possible[j] == 0 && grid[i].value == 0) possible_check++;
            // si compteur à 9 (valeur à 0 et aucune possibilité -> impossible, return 0
            if (possible_check == 9) return 0;
        }
    }
    //printf("Valid grid\n");
    return 1;
}

void backPlay(void) {
    // Remonte l'historique à l'envers jusqu'à la première supposition
    while (history_index > 0) {
        history_index--;
        Affectation *hist = &history[history_index];

        if (hist->supposed == 0) {
            // Déduction : efface simplement la case
            hist->tile->value = 0;
            for (int d = 0; d < 9; d++)
                hist->tile->possible[d] = 1;

        } else {
            // supposition fausse trouvée
            SudokuTiles *t = hist->tile;
            char bad_val   = hist->value;

            // efface la case supposée
            t->value = 0;
            for (int d = 0; d < 9; d++)
                t->possible[d] = 1;

            // réinitialise les possibilités de toutes les cases inconnues
            for (int i = 0; i < GRID_SIZE; i++) {
                if (grid[i].value != 0) continue;
                for (int d = 0; d < 9; d++)
                    grid[i].possible[d] = 1;
            }

            // reconstruit les possibilités par déduction
            cleanGrid();

            // marque la valeur supposée comme impossible
            t->possible[bad_val - 1] = 0;

            return;
        }
    }
}