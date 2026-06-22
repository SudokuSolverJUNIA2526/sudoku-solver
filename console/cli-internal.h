//
// Created by Xan Delayat on 16/06/2026.
//

#ifndef SUDO_SOLV_CLI_INTERNAL_H
#define SUDO_SOLV_CLI_INTERNAL_H

typedef struct {
    char active;
    long nb_deductions;
    long nb_guesses;
    long nb_backplays;
} Verbose;

static Verbose verbose_state = {0, 0, 0, 0};


#endif //SUDO_SOLV_CLI_INTERNAL_H