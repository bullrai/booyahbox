#include "button_config.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "config.h"
#include <string.h>

// --- Variables globales ---
uint8_t buttonPins[BUTTON_COUNT];

uint8_t X_LEFT_PIN, X_RIGHT_PIN, Y_UP_PIN, Y_DOWN_PIN;

// Recherche un GPIO selon un label
uint8_t find_gpio_by_label(const char* label) {
    for (int i = 0; i < 28; ++i) {
        if (strcmp(PROFILE.buttons[i].label, label) == 0)
            return PROFILE.buttons[i].gpio;
    }
    return 0xFF;
}

// Mapping HID : label → index HID dans buttonPins[]
const struct {
    const char* label;
    int index;
} hid_mapping[BUTTON_COUNT] = {
    {"A", 0}, {"B", 1}, {"C", 2}, {"X", 3},
    {"Y", 4}, {"Z", 5}, {"L1", 6}, {"R1", 7},
    {"L2", 8}, {"R2", 9}, {"SELECT", 10}, {"START", 11},
    {"HOME", 12}, {"R3", 13}, {"L3", 14}, {"CAPTURE", 15}
};

void apply_config_btn() {
    for (int i = 0; i < BUTTON_COUNT; ++i) {
        buttonPins[hid_mapping[i].index] = find_gpio_by_label(hid_mapping[i].label);
    }

    X_LEFT_PIN  = find_gpio_by_label("LEFT");
    X_RIGHT_PIN = find_gpio_by_label("RIGHT");
    Y_UP_PIN    = find_gpio_by_label("UP");
    Y_DOWN_PIN  = find_gpio_by_label("DOWN");
}

void setup_pins() {
    for (int i = 0; i < 28; ++i) {
        if (strcmp(PROFILE.buttons[i].label, "NUL") != 0) {
            gpio_init(PROFILE.buttons[i].gpio);
            gpio_set_dir(PROFILE.buttons[i].gpio, GPIO_IN);
            gpio_pull_up(PROFILE.buttons[i].gpio);
        }
    }

}
