// --- config.cpp ---
#include "config.h"

Profile PROFILE = {
    .name = "Booyah Default",
    .buttons = {
        { 0, "NUL" },
        { 1, "NUL" },
        { 2, "LEFT" },
        { 3, "RIGHT" },
        { 4, "UP" },
        { 5, "DOWN" },
        { 6, "A" },
        { 7, "B" },
        { 8, "Y" },
        { 9, "X" },
        { 10, "L1" },
        { 11, "R1" },
        { 12, "L2" },
        { 13, "R2" },
        { 14, "SELECT" },
        { 15, "START" },
        { 16, "HOME" },
        { 17, "R3" },
        { 18, "L3" },
        { 19, "CAPTURE" }, // inutilisé
        { 20, "NUL" },
        { 21, "NUL" },
        { 22, "NUL" },
        { 23, "NUL" },
        { 24, "NUL" },
        { 25, "NUL" },
        { 26, "NUL" },
        { 27, "NUL" },
    },

    .use_dpad_mode = true,
    .socd_enabled = true,
    .socd_strategy = NEUTRAL // CHOIX NEUTRAL, UP_PRIORITY, FIRST_INPUT_PRIORITY, LAST_INPUT_PRIORITY
};

