#include "hotkey.h"
#include "socd.h"
#include "pins.h"





// Variables d’état pour le combo
static bool combo_active = false;
static absolute_time_t combo_start;

// --- Reboot en mode bootsel avec combo de touche ---
void input_reboot_bootsel() {
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

void input_change_scocd_mode() {

    // Vérifier si le bouton de changement de stratégie est enfoncé
    if (!gpio_get(BUTTON_SELECT_PIN) && !gpio_get(Y_UP_PIN)) {
        if (!combo_active) {
            // première détection de la combinaison
            combo_active = true;
            combo_start  = get_absolute_time();
        } else {
            // déjà en mode “combo”, on vérifie la durée
            int64_t dt_us = absolute_time_diff_us(combo_start, get_absolute_time());
            if (dt_us >= 3 * 1000000) {
                // 3 secondes écoulées → reboot en BOOTSEL
                change_socd_strategy();
                // note : le code ne revient jamais ici, la puce redémarre
            }
        }
    } else {
        // relâchement de l’un des deux boutons : on réarme
        combo_active = false;
    }
    
}

// void switch_mode_input() {
//     bool start = !gpio_get(BUTTON_START_PIN);
//     bool left  = !gpio_get(X_LEFT_PIN);
//     bool right = !gpio_get(X_RIGHT_PIN);
    
//     if (start && left && right) {
//         if (!combo_active) {
//             combo_active = true;
//             combo_start  = get_absolute_time();
            
//         } else {
//             // déjà en mode “combo”, on vérifie la durée
//             int64_t dt_us = absolute_time_diff_us(combo_start, get_absolute_time());
//             if (dt_us >= 3 * 1000000) {
//                 // 3 secondes écoulées → reboot en BOOTSEL
//                 currentMode = (currentMode==MODE_GAME?MODE_CONFIG:MODE_GAME);
//                 // note : le code ne revient jamais ici, la puce redémarre
//             }
//         }
//     } else {
//         // relâchement de l’un des deux boutons : on réarme
//         combo_active = false;
//     }
// }