//
// Created by Xan Delayat on 27/05/2026.
//

#include "cli.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "stdlib.h"
#include "../console/cli.h"

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
        fflush(stdout);
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
                return 1;
            }
            dispFinal();
        } else if (strncmp(cmd, "-g ", 3) == 0 || strncmp(cmd, "--generate ", 11) == 0) {
            char *arg = strchr(cmd, ' ');

            if (arg == NULL) {
                fprintf(stderr, "ERROR: Missing level (-g <int level>)\n");
                continue;
            }

            // sauter les espaces
            while (*arg == ' ') arg++;

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

            if (arg == NULL) {
                fprintf(stderr, "ERROR: Missing seed value.\n");
                continue;
            }

            while (*arg == ' ')
                arg++;

            char *endptr;
            unsigned int seed = strtol(arg, &endptr, 10);

            if (*endptr != '\0') {
                fprintf(stderr, "ERROR: Invalid seed '%s'\n", arg);
            }


        } else if (strcmp(cmd, "-v") == 0 || strcmp(cmd, "--verbose") == 0) {
            /* verbose = 1; */
        } else if (strcmp(cmd, "-b") == 0 || strcmp(cmd, "--benchmark") == 0) {
            /* runBenchmark(); */
        } else if (strcmp(cmd, "-i") == 0 || strcmp(cmd, "--interactive") == 0) {
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

unsigned short seedGen(unsigned short seed) {
}

char verbose() {
}

char benchmark() {
}

char interactive() {
}
