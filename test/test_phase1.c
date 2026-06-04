//
// Created by Joan DELAYAT on 03/06/2026.
//

#include <stdio.h>
#include <string.h>
#include "test_framework.h"

/* Inclure les modules à tester */
#include "../entity/sudoku_types.h"
#include "../entity/grid.h"
#include "../repository/io.h"
//Pour test unitaire
/* ── init_grid ─────────────────────────────────────────────── */
static void test_init_grid_all_unknown(void) {
    init_grid();
    for (int i = 0; i < 81; i++) {
        ASSERT_EQ(grid[i].value, 0, "Toutes les cases doivent être inconnues");
    }
}

static void test_init_grid_all_possible(void) {
    init_grid();
    for (int i = 0; i < 81; i++) {
        for (int v = 0; v < 9; v++) {
            ASSERT_NEQ(grid[i].possible[v], 0,
                       "Toutes les valeurs doivent être possibles");
        }
    }
}

/* ── set_tile_value ────────────────────────────────────────── */
static void test_set_tile_value_sets_value(void) {
    init_grid();
    set_tile_value(&grid[0], 5, 0);
    ASSERT_EQ(grid[0].value, 5, "La valeur doit être 5");
}

static void test_set_tile_value_clears_possible(void) {
    init_grid();
    set_tile_value(&grid[0], 3, 0);
    /* Les valeurs != 3 doivent rester possibles en interne,
       mais la case est fixée : seule la valeur 3 est "possible" */
    ASSERT_EQ(grid[0].possible[2], 1,
              "possible[2] (valeur 3) doit rester à 1");
    for (int v = 0; v < 9; v++) {
        if (v != 2)
            ASSERT_EQ(grid[0].possible[v], 0,
                      "Les autres valeurs ne doivent plus être possibles");
    }
}

/* ── load_grid_from_file ───────────────────────────────────── */
static void test_load_grid_from_file_valid(void) {
    init_grid();
    char ret = load_grid_from_file("tables/trivial_table_1.txt");
    ASSERT_EQ(ret, 1, "Chargement d'un fichier valide doit retourner 1");
    /* La case (A,1) vaut 7 selon l'extrait du sujet */
    ASSERT_EQ(grid[0].value, 7, "grid[0] doit valoir 7");
}

static void test_load_grid_from_file_unknown_cell(void) {
    init_grid();
    load_grid_from_file("tables/trivial_table_1.txt");
    /* La case (A,2) est '?' dans le fichier */
    ASSERT_EQ(grid[1].value, 0, "Une case '?' doit rester inconnue (value=0)");
}

static void test_load_grid_from_file_missing(void) {
    init_grid();
    char ret = load_grid_from_file("tables/fichier_inexistant.txt");
    ASSERT_EQ(ret, 0, "Un fichier manquant doit retourner 0");
}

/* ── save_grid_to_file ─────────────────────────────────────── */
static void test_save_then_reload(void) {
    init_grid();
    load_grid_from_file("tables/trivial_table_1.txt");
    char ret_save = save_grid_to_file("/tmp/test_save.txt");
    ASSERT_EQ(ret_save, 1, "La sauvegarde doit réussir");

    /* Recharger et comparer la première case connue */
    char val_avant = grid[0].value;
    init_grid();
    load_grid_from_file("/tmp/test_save.txt");
    ASSERT_EQ(grid[0].value, val_avant,
              "La valeur rechargée doit correspondre à celle sauvegardée");
}

/* ── Cohérence globale ─────────────────────────────────────── */
static void test_grid_has_81_cells(void) {
    /* Vérifie que la grille contient bien 81 cases accessibles */
    init_grid();
    /* Accès à la dernière case : grid[80] = case (9,9) */
    set_tile_value(&grid[80], 9, 0);
    ASSERT_EQ(grid[80].value, 9, "La case grid[80] doit être accessible");
}

/* ── main ──────────────────────────────────────────────────── */
int main(void) {
    printf("=== Phase 1 — Tests unitaires ===\n\n");

    RUN_TEST(test_init_grid_all_unknown);
    RUN_TEST(test_init_grid_all_possible);
    RUN_TEST(test_set_tile_value_sets_value);
    RUN_TEST(test_set_tile_value_clears_possible);
    RUN_TEST(test_load_grid_from_file_valid);
    RUN_TEST(test_load_grid_from_file_unknown_cell);
    RUN_TEST(test_load_grid_from_file_missing);
    RUN_TEST(test_save_then_reload);
    RUN_TEST(test_grid_has_81_cells);

    PRINT_RESULTS();
    EXIT_RESULTS();
}