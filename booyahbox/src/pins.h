#pragma once

// Joystick via boutons (leverless style)
#define X_LEFT_PIN     2
#define X_RIGHT_PIN    3
#define Y_UP_PIN       4
#define Y_DOWN_PIN     5

// --- Pins pour Boutons (14 boutons) ---

#define BUTTON_A_PIN      6
#define BUTTON_X_PIN      7
#define BUTTON_Y_PIN      8
#define BUTTON_B_PIN      9
#define BUTTON_LB_PIN    10
#define BUTTON_RB_PIN    11
#define BUTTON_LT_PIN    12
#define BUTTON_RT_PIN    13
#define BUTTON_SELECT_PIN 14
#define BUTTON_START_PIN  15
#define BUTTON_LS_PIN     16
#define BUTTON_RS_PIN     17
#define BUTTON_HOME_PIN   18
#define BUTTON_CAPTURE_PIN 19



// --- Tableau pour boucle ---
#define BUTTON_COUNT 16
const uint8_t buttonPins[BUTTON_COUNT] = {
    BUTTON_A_PIN,
    BUTTON_B_PIN,
    22,
    BUTTON_X_PIN,
    BUTTON_Y_PIN,
    23,
    BUTTON_LB_PIN,
    BUTTON_RB_PIN,
    BUTTON_LT_PIN,
    BUTTON_RT_PIN,
    BUTTON_SELECT_PIN,
    BUTTON_START_PIN,
    BUTTON_LS_PIN,
    BUTTON_RS_PIN,
    BUTTON_HOME_PIN,
    BUTTON_CAPTURE_PIN
};
