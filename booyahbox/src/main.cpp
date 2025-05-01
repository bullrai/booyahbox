// BouyahBox Firmware v0.1 — Gamepad HID 20 boutons

#include "tusb_config.h"
#include "tusb.h"
#include "pico/stdlib.h"
#include "bsp/board.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/bootrom.h"
#include "hardware/timer.h"

#include "pins.h"
#include "usb_descriptors.cpp"
#include "board_setup.cpp"
// #include "action_key.cpp"

// --- Initialisation du report HID ---
struct __attribute__((packed)) GamepadReport {
    int8_t x;
    int8_t y;
    uint16_t buttonss;  // 16 boutons 
};


void setup_pins();

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



int8_t read_axis(uint pin_neg, uint pin_pos) {
    bool neg = !gpio_get(pin_neg);
    bool pos = !gpio_get(pin_pos);
    if (neg && !pos) return -127;
    if (pos && !neg) return 127;
    return 0;
}

// --- Lecture du D-Pad (hat switch) ---
uint8_t read_hat(uint gpio_up, uint gpio_down, uint gpio_left, uint gpio_right) {
    bool up    = !gpio_get(X_LEFT_PIN);
    bool down  = !gpio_get(X_RIGHT_PIN);
    bool left  = !gpio_get(Y_UP_PIN);
    bool right = !gpio_get(Y_DOWN_PIN);

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
// Variables d’état pour le combo
static bool combo_active = false;
static absolute_time_t combo_start;



// Lit tes boutons et axes, puis appelle cette fonction dans la boucle
void process_buttons_and_maybe_reboot() {
    // lire l’état 16-bit de tous les boutons

    bool start  = !gpio_get(BUTTON_START_PIN);
    bool select = !gpio_get(BUTTON_SELECT_PIN);

    if (start && select) {
        if (!combo_active) {
            // première détection de la combinaison
            combo_active = true;
            combo_start  = get_absolute_time();
        } else {
            // déjà en mode “combo”, on vérifie la durée
            int64_t dt_us = absolute_time_diff_us(combo_start, get_absolute_time());
            if (dt_us >= 3 * 1000000) {
                // 3 secondes écoulées → reboot en BOOTSEL
                reset_usb_boot(0, 0);
                // note : le code ne revient jamais ici, la puce redémarre
            }
        }
    } else {
        // relâchement de l’un des deux boutons : on réarme
        combo_active = false;
    }
}

// --- Main program ---
int main() {
    board_init();
    tusb_init();
    setup_pins();

    while (true) {
        tud_task();

        process_buttons_and_maybe_reboot();
        
        GamepadReport report = {};
        report.x = read_axis(X_LEFT_PIN, X_RIGHT_PIN); // -127 à 127
        report.y = read_axis(Y_UP_PIN, Y_DOWN_PIN);; // -127 à 127
        report.buttonss = read_buttons();
        

        if (tud_hid_ready()) {
            tud_hid_report(0, &report, sizeof(report));
        }

        sleep_ms(5);
    }
}
