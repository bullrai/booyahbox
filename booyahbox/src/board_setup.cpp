#include "pins.h"
#include "tusb.h"

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
    BUTTON_HOME_PIN,
    BUTTON_RS_PIN,
    BUTTON_LS_PIN,
    BUTTON_CAPTURE_PIN,
    
};





// --- Initialisation des broches ---
void setup_pins() {
    gpio_init(X_LEFT_PIN);   gpio_set_dir(X_LEFT_PIN, GPIO_IN);   gpio_pull_up(X_LEFT_PIN);
    gpio_init(X_RIGHT_PIN);  gpio_set_dir(X_RIGHT_PIN, GPIO_IN);  gpio_pull_up(X_RIGHT_PIN);
    gpio_init(Y_UP_PIN);     gpio_set_dir(Y_UP_PIN, GPIO_IN);     gpio_pull_up(Y_UP_PIN);
    gpio_init(Y_DOWN_PIN);   gpio_set_dir(Y_DOWN_PIN, GPIO_IN);   gpio_pull_up(Y_DOWN_PIN);

    // Buttons setup
    for (int i = 0; i < BUTTON_COUNT; i++) {
        gpio_init(buttonPins[i]);
        gpio_set_dir(buttonPins[i], GPIO_IN);
        gpio_pull_up(buttonPins[i]);
    }
}