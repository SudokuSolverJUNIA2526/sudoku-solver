#include "io.h"
#include <stdio.h>

char loadGridFromFile(const char *path) {
    if (path == NULL) {
        fprintf(stderr, "loadGridFromFile: path NULL.\n");
        return 0;
    }
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "loadGridFromFile: impossible to open '%s'.\n", path);
        return 0;
    }
    initGrid();
    int row = 1, col = 1;
    while (row <= 9) {
        int c = fgetc(f);
        if (c == EOF) {
            fprintf(stderr, "loadGridFromFile: unexpected EOF (row %d, col %d).\n", row, col);
            fclose(f);
            return 0;
        }
        if (c >= '1' && c <= '9') {
            setTileValue(&grid[(row-1)*9 + (col-1)], (char)(c - '0'), 0);
            col++;
        } else if (c == '?') {
            col++;
        } else if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            /* séparateurs ignorés */
        } else {
            fprintf(stderr, "loadGridFromFile: unexpected char '%c' (row %d).\n", (char)c, row);
            fclose(f);
            return 0;
        }
        if (col > 9) { col = 1; row++; }
    }
    fclose(f);
    return 1;
}

char saveGridToFile(const char *path) {
    if (path == NULL) {
        fprintf(stderr, "saveGridToFile: path NULL.\n");
        return 0;
    }
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        fprintf(stderr, "saveGridToFile: cannot open '%s'.\n", path);
        return 0;
    }
    for (int row = 1; row <= 9; row++) {
        for (int col = 1; col <= 9; col++) {
            SudokuTiles *t = &grid[(row-1)*9 + (col-1)];
            fprintf(f, "%c", t->value != 0 ? '0' + t->value : '?');
            if (col < 9) fprintf(f, " ");
        }
        fprintf(f, "\n");
    }
    fclose(f);
    return 1;
}

void reqStartGrid(void) {
    printf("Input grid (9 lines, digits 1-9 or '?' for unknown):\n");
    initGrid();
    int row = 1, col = 1;
    while (row <= 9) {
        int c = fgetc(stdin);
        if (c == EOF) break;
        if (c == '\n') continue;
        if (c != '?' && (c < '1' || c > '9')) continue;
        if (c >= '1' && c <= '9')
            setTileValue(&grid[(row-1)*9 + (col-1)], (char)(c - '0'), 0);
        col++;
        if (col > 9) { col = 1; row++; }
    }
    printf("Grid loaded.\n");
}