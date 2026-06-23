#include "io.h"
#include <stdio.h>
#include <windows.h>

// charge une grille depuis un fichier texte
// le fichier doit contenir 9 lignes de 9 symboles séparés par des espaces
// chaque symbole est un chiffre de 1 à 9 ou un ? pour une case vide
// retourne 1 si le chargement a réussi, 0 en cas d'erreur
char loadGridFromFile(const char *path) {
    if (path == NULL) {
        fprintf(stderr, "  loadGridFromFile: path NULL.\n");
        return 0;
    }
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        Sleep(1000);
        fprintf(stderr, "  loadGridFromFile: impossible to open '%s'. (Non existing path or mistyped ?)\n", path);
        return 0;
    }
    // remet la grille à zéro avant de la remplir
    initGrid();
    int row = 1, col = 1;
    while (row <= 9) {
        int c = fgetc(f);
        if (c == EOF) {
            fprintf(stderr, "  loadGridFromFile: unexpected EOF (row %d, col %d).\n", row, col);
            fclose(f);
            return 0;
        }
        if (c >= '1' && c <= '9') {
            // case connue : on fixe la valeur dans la grille
            setTileValue(&grid[(row-1)*9 + (col-1)], (char)(c - '0'), 0);
            col++;
        } else if (c == '?') {
            // case inconnue : déjà initialisée par initGrid(), on passe juste à la suivante
            col++;
        } else if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            // séparateurs ignorés, on ne compte pas de case
        } else {
            fprintf(stderr, "  loadGridFromFile: unexpected char '%c' (row %d).\n", (char)c, row);
            fclose(f);
            return 0;
        }
        // quand on a lu 9 cases, on passe à la ligne suivante
        if (col > 9) { col = 1; row++; }
    }
    fclose(f);
    // fprintf(stdout, "File %s successfully loaded.\n", path);
    return 1;
}

// sauvegarde la grille courante dans un fichier texte
// les cases connues sont écrites avec leur chiffre, les cases inconnues avec un ?
// retourne 1 si la sauvegarde a réussi, 0 en cas d'erreur
char saveGridToFile(const char *path) {
    if (path == NULL) {
        fprintf(stderr, "  saveGridToFile: path NULL.\n");
        return 0;
    }
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        fprintf(stderr, "  saveGridToFile: cannot open '%s'.\n", path);
        return 0;
    }
    for (int row = 1; row <= 9; row++) {
        for (int col = 1; col <= 9; col++) {
            SudokuTiles *t = &grid[(row-1)*9 + (col-1)];
            // écrit la valeur ou un ? si la case est inconnue
            fprintf(f, "%c", t->value != 0 ? '0' + t->value : '?');
            // sépare les cases par un espace sauf en fin de ligne
            if (col < 9) fprintf(f, " ");
        }
        fprintf(f, "\n");
    }
    printf("  File saved to: %s\n", path);
    fclose(f);
    return 1;
}

// demande à l'utilisateur de saisir une grille au clavier ligne par ligne
// accepte les chiffres de 1 à 9 et le ? pour les cases vides
// tout autre caractère est ignoré sauf le retour à la ligne
void reqStartGrid(void) {
    printf("Input grid (9 lines, digits 1-9 or '?' for unknown):\n");
    // remet la grille à zéro avant la saisie
    initGrid();
    int row = 1, col = 1;
    while (row <= 9) {
        int c = fgetc(stdin);
        if (c == EOF) break;
        // les retours à la ligne sont ignorés, on compte les cases pas les lignes
        if (c == '\n') continue;
        // on ignore tout ce qui n'est ni chiffre ni ?
        if (c != '?' && (c < '1' || c > '9')) continue;
        // case connue : on fixe la valeur, case ? : déjà initialisée par initGrid()
        if (c >= '1' && c <= '9')
            setTileValue(&grid[(row-1)*9 + (col-1)], (char)(c - '0'), 0);
        col++;
        // passage à la ligne suivante après 9 cases
        if (col > 9) { col = 1; row++; }
    }
    printf("Grid loaded.\n");
}