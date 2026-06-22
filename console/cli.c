//
// Created by Xan Delayat on 27/05/2026.
//

#include "cli.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "stdlib.h"
#include "time.h"
#include "../console/cli.h"
#include "dirent.h"

#include "../entity/generator.h"
#include "../repository/io.h"
#include "../entity/sudoku_types.h"
#include "../service/service_solver.h"

static void printHelp(const char *cmd) {
    fprintf(stdout,
            "\n\nCommand: %s is not a valid command, please refer to the help below:\n"
            "  -h, --help            Display this help interface\n"
            "  -l, --load FILE       Load and display a grid from FILE\n"
            "  -g  --generate LEVEL  Generate a grid with the difficulty you wish\n"
            "  -s  --seed N          Random grid generator's seed\n"
            "  -v  --verbose         Detailed track of the resolution\n"
            "  -b  --benchmark       Execute benchmark\n"
            "  -i, --interactive     Input a grid\n"
            "  -e  --exit            Exit the program\n",
            cmd);
}

static void inputCmd() {
    fprintf(stdout, ">\t");
}

char cli(void) {
    char cmd[256];
    unsigned short stop = 0;

    static unsigned int current_seed = 0;
    static int seed_set = 0;

    while (stop == 0) {
        fprintf(stdout, "Enter command ");
        inputCmd();
        // lit toute la ligne dans cmd
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) break;
        // retire le '\n' en fin de chaîne
        int len = (int) strlen(cmd);
        if (len > 0 && cmd[len - 1] == '\n') cmd[len - 1] = '\0';

        if (strcmp(cmd, "-h") == 0 || strcmp(cmd, "--help") == 0) {
            printHelp(cmd);
        } else if (strncmp(cmd, "-l ", 3) == 0 || strncmp(cmd, "--load ", 7) == 0) {
            char *path = strchr(cmd, ' ');
            while (*path == ' ') path++;
            if (!loadGridFromFile(path)) {
                fprintf(stderr, "Error loading grid: %s\n", path);
                continue;
            }
            // active le callback si verbose est ON
            if (verbose_state.active) setTraceCallback(traceVerbose);
            buildAllSubsets();
            char solved = resolveGrid();
            setTraceCallback(NULL);
            printf("Result : %s\n", solved ? "solved" : "incomplete");

            dispFinal();
            freeAllSubsets();
        } else if (strncmp(cmd, "-g ", 3) == 0 || strncmp(cmd, "--generate ", 11) == 0) {
            char *arg = strchr(cmd, ' ');

            if (arg == NULL) {
                fprintf(stderr, "ERROR: Missing level (-g <int level>)\n");
                continue;
            }

            // sauter les espaces
            while (*arg == ' ') arg++;

            if (strcmp(arg, "-h") == 0) {
                generateLevelHelp();
                continue;
            }

            difficulty_t level;
            level.difficulty = 0;

            // attribution valeur difficulté à la génération
            if (strcmp(arg, "1") == 0 || strcmp(arg, "--trivial") == 0)
                level.difficulty = 1;
            else if (strcmp(arg, "2") == 0 || strcmp(arg, "--basic") == 0)
                level.difficulty = 2;
            else if (strcmp(arg, "3") == 0 || strcmp(arg, "--intermediate") == 0)
                level.difficulty = 3;
            else if (strcmp(arg, "4") == 0 || strcmp(arg, "--hard") == 0)
                level.difficulty = 4;
            else if (strcmp(arg, "5") == 0 || strcmp(arg, "--expert") == 0)
                level.difficulty = 5;
            else if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0)
                generateLevelHelp();
            else {
                fprintf(stderr, "ERROR: Invalid difficulty '%s'\n", arg);
            }
            if (level.difficulty != 0) generateGrid(level, current_seed);
            dispFinal();

        } else if (strncmp(cmd, "-s ", 3) == 0 || strncmp(cmd, "--seed ", 7) == 0) {
            char *arg = strchr(cmd, ' ');

            //si pas de seed
            if (arg == NULL) {
                fprintf(stderr, "ERROR: Missing seed value.\n");
                continue;
            } else {
                // sauter tout les espaces
                while (*arg == ' ')
                    arg++;

                // seed set à valeur en base décimale, conversion de arg en int
                char *endptr;
                current_seed = strtol(arg, &endptr, 10);

                // si seed non int
                if (endptr == arg || (*endptr != '\0' && *endptr != '\n')) {
                    fprintf(stderr, "ERROR: Invalid seed '%s'\n", arg);
                    continue;
                }

                fprintf(stdout, "Seed value set to %d\n", current_seed);
                seed_set = 1;
            }

        } else if (strcmp(cmd, "-v") == 0 || strcmp(cmd, "--verbose") == 0) {
            verbose();
        } else if (strcmp(cmd, "-b") == 0 || strcmp(cmd, "--benchmark") == 0) {
            benchmark();
        } else if (strcmp(cmd, "-i") == 0 || strcmp(cmd, "--interactive") == 0 || strcmp(cmd, "") == 0 ) {
            interactive();
        } else if (strcmp(cmd, "-e") == 0 || strcmp(cmd, "--exit") == 0) {
            stop = 1;
        } else {
            fprintf(stderr,
                    "Unknown command: '%s'. Input -h for more infos.\n",
                    cmd);
        }
    }
    return 0;
}

static void generateLevelHelp() {
    fprintf(stdout,
            "Level generation difficulty help: \n"
            "  1, --trivial\n"
            "  2, --basic\n"
            "  3, --intermediate\n"
            "  4, --hard\n"
            "  5, --expert\n\n");
}

char verbose(void) {
    verbose_state.active = !verbose_state.active;
    fprintf(stdout, "Verbose mode: %s.\n", verbose_state.active ? "ON" : "OFF");
    return verbose_state.active;
}

static void traceVerbose(int row, int col, char val, char supposed) {
    verbose_state.active ? supposed
        ? printf("[verbose] guess      : case (%d,%d) = %d\n", row, col, val)
        : printf("[verbose] deduction  : case (%d,%d) = %d\n", row, col, val)
        : (void) 0;
    if (supposed == 0) verbose_state.nb_deductions++;
    else               verbose_state.nb_guesses++;
}

static void traceCount(int row, int col, char val, char supposed) {
    (void)row; (void)col; (void)val; /* parametres non utilises */
    if (supposed == 0) verbose_state.nb_deductions++;
    else               verbose_state.nb_guesses++;
}


char benchmark(void) {
    DIR *d = opendir("filebase");
    if (d == NULL) {
        fprintf(stderr, "[benchmark] impossible d'ouvrir le repertoire 'filebase/'.\n");
        return 0;
    }

    printf("%-35s | %10s | %12s | %12s | %s\n",
           "File", "Time (ms)", "Deductions", "Guesses", "Solved");
    printf("-----------------------------------------------------------------------------------------------\n");

    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        // on ne traite que les fichiers .txt
        size_t len = strlen(entry->d_name);
        if (len < 4 || strcmp(entry->d_name + len - 4, ".txt") != 0)
            continue;

        char path[256];
        snprintf(path, sizeof(path), "filebase/%s", entry->d_name);

        if (!loadGridFromFile(path)) {
            printf("%-35s | %10s\n", entry->d_name, "erreur");
            continue;
        }

        // reset des stats pour cette grille
        verbose_state.nb_deductions = 0;
        verbose_state.nb_guesses    = 0;

        // active le callback pour compter sans afficher
        setTraceCallback(traceCount);
        buildAllSubsets();

        clock_t start = clock();
        char solved = resolveGrid();
        clock_t end = clock();

        setTraceCallback(NULL);
        freeAllSubsets();

        double ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;

        // pour affichage dans le tableau des détails de résolution
        printf("%-35s | %10.2f | %12ld | %12ld | %s\n",
               entry->d_name, ms,
               verbose_state.nb_deductions,
               verbose_state.nb_guesses,
               solved ? "oui" : "non");
    }

    closedir(d);
    return 1;
}

static void interactiveHelp() {
    fprintf(stdout,
        "Interactive mode help:\n"
        "  -h   --help              Display command help\n"
        "  -w,  --write             Manually input a grid\n"
        "  -s   --solve  FILE       Activate the sudoku solver\n"
        "  -p   --possibles         Display different value possibilities\n"
        "  -d   --display           Display current grid state\n"
        "  -e   --exit              Exit interactive mode\n"
        );
}

char interactive() {
    char exit = 1;
    char cmd[256];

    fprintf(stdout, "Entered interactive mode.\n");
    while (exit == 1) {
        fprintf(stdout, "Enter a command");
        inputCmd();
        fflush(stdout);
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) break;
        int len = (int) strlen(cmd);
        if (len > 0 && cmd[len - 1] == '\n') cmd[len - 1] = '\0';

        if (strcmp(cmd, "-h") == 0 || strcmp(cmd, "--help") == 0) {
            interactiveHelp();
        } else if (strcmp(cmd, "-w") == 0 || strcmp(cmd, "--write") == 0) {
            difficulty_t level;
            level.difficulty = 0;
            char *diff = "";
            fprintf(stdout, "Set difficulty value: ");
            if (fgets(diff, sizeof(diff), stdin) == NULL) break;
            level.difficulty = strtol(diff, &diff, 10);

            reqStartGrid();

            // retrait des cases en fonction de la difficulté
            int nb_remove;
            switch (level.difficulty) {
                case 1:
                    nb_remove = 30;
                    break;
                case 2:
                    nb_remove = 40;
                    break;
                case 3:
                    nb_remove = 50;
                    break;
                case 4:
                    nb_remove = 60;
                    break;
                default:
                    nb_remove = 70;
                    break;
            }

            int removed = 0;
            int attempts = 0;
            while (removed < nb_remove && attempts < 1000) {
                int index_rm = rand() % 81;
                attempts++;
                // cherche une case encore remplie
                if (grid[index_rm].value == 0) continue;
                grid[index_rm].value = 0;
                for (int d = 0; d < 9; d++)
                    grid[index_rm].possible[d] = 1;
                removed++;
            }
        } else if (strcmp(cmd, "-s") == 0 || strcmp(cmd, "--solve") == 0) {
            resolveGrid();
            fprintf(stdout, "Grid has been solved\n");
            dispFinal();
        } else if (strcmp(cmd, "-p") == 0 || strcmp(cmd, "--possibles") == 0) {
            dispPossible();
        } else if (strcmp(cmd, "-d") == 0 || strcmp(cmd, "--display") == 0) {
            dispFinal();
        } else if (strcmp(cmd, "-e") == 0 || strcmp(cmd, "--exit") == 0) {
            exit = 0;
        }
    }
    return 0;
}
