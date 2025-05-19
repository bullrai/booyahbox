
#include "hotkey.h"
#include "pico/stdlib.h"
#include "config.h"
#include "socd.h"
#include "input.h"
#include "button_config.h"
#include "tusb.h"
#include <stdio.h>
#include <string.h>

// --- Callback externes ---
// extern void reboot_bootsel();
extern void change_socd_strategy();
extern void apply_socd_neutral();
extern void apply_socd_first_input_priority();
extern void apply_socd_last_input_priority();
extern void apply_socd_up_priority();
extern void change_Dpad_mode();

// --- Définition d'une combinaison de touches ---
struct HotkeyAction {
    const char* label1;
    const char* label2;
    uint32_t delay_ms;
    void (*callback)();
    bool active;
    absolute_time_t start;
};

// --- Définition locale des boutons ---
struct HotkeyButton {
    const char* label;
    uint8_t gpio;
};

HotkeyButton local_buttons[28];

void copy_buttons_from_profile() {
    for (int i = 0; i < 28; ++i) {
        local_buttons[i].label = PROFILE.buttons[i].label;
        local_buttons[i].gpio = PROFILE.buttons[i].gpio;
    }
}

void reboot_bootsel() {
        reset_usb_boot(0, 0);
}

// --- Table des hotkeys ---
HotkeyAction hotkeys[] = {
    { "START",  "SELECT", 3000, reboot_bootsel, false },
    { "SELECT", "DOWN",   1000, apply_socd_neutral, false },
    { "SELECT", "LEFT",   1000, apply_socd_first_input_priority, false },
    { "SELECT", "RIGHT",  1000, apply_socd_last_input_priority, false },
    { "SELECT", "UP",     1000, apply_socd_up_priority, false },
    { "R3",     "L3",     1000, change_Dpad_mode, false }
    
};

// --- Vérifie si une touche est pressée (GPIO LOW) ---
bool is_button_pressed(const char* label) {
    for (int i = 0; i < 28; ++i) {
        if (strcmp(local_buttons[i].label, label) == 0) {
            return !gpio_get(local_buttons[i].gpio);
        }
    }
    return false;
}

// --- Vérifie que SEULES les 2 touches sont pressées ---
bool only_buttons_pressed(const char* label1, const char* label2) {
    for (int i = 0; i < 28; ++i) {
        if (strcmp(local_buttons[i].label, "NUL") == 0) continue;

        bool pressed = !gpio_get(local_buttons[i].gpio);
        if (pressed) {
            if (strcmp(local_buttons[i].label, label1) != 0 && strcmp(local_buttons[i].label, label2) != 0) {
                return false; // un autre bouton est pressé
            }
        }
    }
    return true;
}



// --- Traitement des hotkeys ---
void update_hotkeys() {
    for (int i = 0; i < sizeof(hotkeys)/sizeof(HotkeyAction); ++i) {
        HotkeyAction& hk = hotkeys[i];
        bool a = is_button_pressed(hk.label1);
        bool b = is_button_pressed(hk.label2);

        if (a && b && only_buttons_pressed(hk.label1, hk.label2)) {
            if (!hk.active) {
                hk.active = true;
                hk.start = get_absolute_time();
            } else {
                int64_t dt = absolute_time_diff_us(hk.start, get_absolute_time());
                if (dt >= hk.delay_ms * 1000) {
                    hk.callback();
                    hk.active = false;
                }
            }
        } else {
            hk.active = false;
        }
    }
}



void input_reboot_bootsel() {
    static bool combo_active = false;
    static absolute_time_t combo_start;
    // lire l’état 16-bit de tous les boutons

    bool start  = !gpio_get(15);
    bool select = !gpio_get(14);

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