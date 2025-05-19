// src/read_input.cpp

#include "hardware/gpio.h"
#include "socd.h"
#include "button_config.h"



// --- Lecture des boutons ---
uint16_t read_buttons() {
    uint16_t buttons = 0;
    for (int i = 0; i < BUTTON_COUNT; i++) {
        if (!gpio_get(buttonPins[i])) {
            buttons |= (1 << i);
        }
    }
    return buttons;
}

// --- Lecture d’un axe avec SOCD ---
int8_t read_axis(uint8_t pin_neg, uint8_t pin_pos, bool &first_input, int8_t &first_dir, bool y) {
    bool neg = !gpio_get(pin_neg);
    bool pos = !gpio_get(pin_pos);
    return apply_socd_cleaning(neg, pos, first_input, first_dir, y);
}

// --- Lecture du D-Pad (hat switch) ---
uint8_t read_hat(uint8_t gpio_up, uint8_t gpio_down, uint8_t gpio_left, uint8_t gpio_right) {
    bool up    = !gpio_get(gpio_up);
    bool down  = !gpio_get(gpio_down);
    bool left  = !gpio_get(gpio_left);
    bool right = !gpio_get(gpio_right);

    if (up && !down && !left && !right)  return 1; // haut
    if (up && right && !down && !left)   return 2; // haut-droite
    if (right && !up && !down && !left)  return 3; // droite
    if (down && right && !up && !left)   return 4; // bas-droite
    if (down && !up && !left && !right)  return 5; // bas
    if (down && left && !up && !right)   return 6; // bas-gauche
    if (left && !up && !down && !right)  return 7; // gauche
    if (up && left && !down && !right)   return 8; // haut-gauche

    return 0; // neutre
}
