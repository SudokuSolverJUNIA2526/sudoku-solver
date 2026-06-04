//
// Created by Xan Delayat on 27/05/2026.
//

#ifndef SUDOKUSOLVER_GENERATOR_INTERNAL_H
#define SUDOKUSOLVER_GENERATOR_INTERNAL_H

typedef struct {
    enum {
        TRIVIAL,
        BASIC,
        INTERMEDIATE,
        HARD,
        EXPERT
    };
} difficulty_t;

#endif //SUDOKUSOLVER_GENERATOR_INTERNAL_H