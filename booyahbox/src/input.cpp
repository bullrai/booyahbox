// --- input.cpp ---
#include "input.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "tusb.h"
#include "config.h"
#include "button_config.h"
#include "socd.h"

bool use_dpad_mode = false; // true = D-Pad, false = Stick

#define NUM_BUTTONS 16
#define DEBOUNCE_THRESHOLD 3 // nombre de cycles stables requis

static uint8_t debounce_counters[GPIO_COUNT] = {0};
static bool last_raw_state[GPIO_COUNT] = {0};
static bool stable_state[GPIO_COUNT] = {0};

bool debounce_gpio(uint8_t gpio) {
    bool current_raw = !gpio_get(gpio);
    uint8_t &counter = debounce_counters[gpio];
    bool &last_state = last_raw_state[gpio];
    bool &stable = stable_state[gpio];

    if (current_raw == last_state) {
        if (counter < DEBOUNCE_THRESHOLD) {
            counter++;
        }
    } else {
        counter = 0;
    }

    if (counter >= DEBOUNCE_THRESHOLD) {
        stable = current_raw;
    }

    last_state = current_raw;
    return stable;
}

uint16_t read_buttons() {
    uint16_t btn = 0;
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (debounce_gpio(buttonPins[i])) {
            btn |= (1 << i);
        }
    }
    return btn;
}

int8_t read_axis(uint8_t pin_neg, uint8_t pin_pos, bool &first_input, int8_t &first_dir, bool y) {
    // if (!use_dpad_mode) return 0; // Ne pas lire les axes si en mode D-Pad
    bool neg = debounce_gpio(pin_neg);
    bool pos = debounce_gpio(pin_pos);
    return apply_socd_cleaning(neg, pos, first_input, first_dir, y);
}

uint8_t read_hat(uint8_t up, uint8_t down, uint8_t left, uint8_t right) {
    if (use_dpad_mode) return 15; 
    bool u = debounce_gpio(up);
    bool d = debounce_gpio(down);
    bool l = debounce_gpio(left);
    bool r = debounce_gpio(right);

    if (u && !d && !l && !r) return 0;
    if (u && r && !d && !l) return 1;
    if (r && !u && !d && !l) return 2;
    if (d && r && !u && !l) return 3;
    if (d && !u && !l && !r) return 4;
    if (d && l && !u && !r) return 5;
    if (l && !u && !d && !r) return 6;
    if (u && l && !d && !r) return 7;
    return 15;// neutre
}


void change_Dpad_mode() {
    PROFILE.use_dpad_mode = use_dpad_mode;
    use_dpad_mode = !use_dpad_mode;
}