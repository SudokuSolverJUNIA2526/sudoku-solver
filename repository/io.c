//
// Created by Xan Delayat on 27/05/2026.
//

#include "io.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

char loadGridFromFile(const char *path) {
    FILE *f;
    int   row, col, c;

    if (path == NULL) {
        fprintf(stderr, "load_grid_from_file: path NULL.\n");
        return 0;
    }

    f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "loadGridFromFile: impossible to open '%s'.\n", path);
        return 0;
    }

    initGrid();

    row = 1;
    col = 1;

    while (row <= 9) {
        c = fgetc(f);

        if (c == EOF) {
            fprintf(stderr,
                    "loadGridFromFile: ending process"
                    " (line %d, column %d).\n", row, col);
            fclose(f);
            return 0;
        }

        if (c >= '1' && c <= '9') {
            setTileValue(&grid[(row - 1) * 9 + (col - 1)],
                           (char)(c - '0'), 0);
            col++;
        } else if (c == '?') {
            // case inconnue: déjà initialisée par init_grid()
            col++;
        } else if (c == ' ' || c == '\t' || c == '\r') {
            // séparateurs ignorés
        } else if (c == '\n') {
            // ignoré également: on compte les colonnes, pas les lignes
        } else {
            fprintf(stderr,
                    "loadGridFromFile: unwanted character '%c'"
                    " (at line %d).\n", (char)c, row);
            fclose(f);
            return 0;
        }

        if (col > 9) {
            col = 1;
            row++;
        }
    }

    fclose(f);
    return 1;
}

char saveGridToFile(const char *path) {
    FILE        *f;
    int          row, col;
    SudokuTiles *t;

    if (path == NULL) {
        fprintf(stderr, "saveGridToFile: NULL.\n");
        return 0;
    }

    f = fopen(path, "w");
    if (f == NULL) {
        fprintf(stderr, "saveGridToFile: impossible to open '%s' on write mode.\n", path);
        return 0;
    }

    for (row = 1; row <= 9; row++) {
        for (col = 1; col <= 9; col++) {
            t = &grid[(row - 1) * 9 + (col - 1)];
            if (t->value != 0) {
                fprintf(f, "%c", '0' + t->value);
            } else {
                fprintf(f, "?");
            }
            if (col < 9) {
                fprintf(f, " ");
            }
        }
        fprintf(f, "\n");
    }

    fclose(f);
    return 1;
}

void reqStartGrid(void) {
    int row, col, c;

    printf("Input values in the grid (9 lines, digits from 1 to 9 or '?' (empty tile) :\n");

    initGrid();

    row = 1;
    col = 1;

    while (row <= 9) {
        c = fgetc(stdin);

        if (c == EOF) {
            break;
        }

        // Ignorer tout ce qui n'est ni chiffre ni '?' ni '\n'
        if (c == '\n') {
            continue;
        }

        if (c != '?' && (c < '1' || c > '9')) {
            continue;
        }

        if (c >= '1' && c <= '9') {
            setTileValue(&grid[(row - 1) * 9 + (col - 1)],
                           (char)(c - '0'), 0);
        }
        // Si '?': case déjà inconnue par init_grid()

        col++;
        if (col > 9) {
            col = 1;
            row++;
        }
    }

    printf("Grid loaded.\n");
}