// remap.cpp
#include "remap.h"
#include "button_config.h"
#include "display.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// --- États du menu de remapping ---


RemapState remap_state = WAIT_BUTTON;
static int current_physical_index = -1;
static int current_selection = 0;
static int current_gpio = 0;

// Entrée utilisateur
static bool up_act = false;
static bool dn_act = false;
static bool ok_act = false;

void process_remap_menu_input() {
    if (currentMode != MODE_CONFIG || currentPage != PAGE_REMAPPING) return;

    bool up      = !gpio_get(Y_UP_PIN) ;
    bool down    = !gpio_get(Y_DOWN_PIN);
    bool confirm = !gpio_get(BUTTON_A_PIN);
    
    switch (remap_state) {
        case WAIT_BUTTON:
            sleep_ms(1000); // Anti-rebond
            for (int i = 0; i < 20; ++i) {
                if (!gpio_get(buttonConfigs[i].gpio)) {
                    current_physical_index = i;
                    current_gpio = buttonConfigs[i].gpio;
                    remap_state = CHOOSE_FUNCTION;
                    break;
                }
            }
            break;

        case CHOOSE_FUNCTION:
            if (up && !up_act) {
                up_act = true;
                if (current_selection > 0) current_selection--;
            } else if (!up) {
                up_act = false;
            }

            if (down && !dn_act) {
                dn_act = true;
                if (current_selection + 1 < 20) current_selection++;
            } else if (!down) {
                dn_act = false;
            }

            if (confirm && !ok_act) {
                ok_act = true;
                buttonConfigs[current_selection].gpio = current_gpio;
                apply_config_btn();
                remap_state = WAIT_BUTTON;
                
            } else if (!confirm) {
                ok_act = false;
            }
            break;
    }
}

RemapState get_remap_state() {
    return remap_state;
}

int get_remap_index() {
    return current_physical_index;
}

int get_remap_selection() {
    return current_selection;
}
