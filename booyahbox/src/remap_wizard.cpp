// remap_wizard.cpp
#include "remap_wizard.h"
#include "button_config.h" // Ensure this header provides the full definition of ButtonConfig

// Define buttonConfigs if not fully defined in the header
extern ButtonConfig buttonConfigs[20]; // Adjust size as needed 
#include "display.h"
#include "pico/stdlib.h"
#include <string.h>



static RemapWizardState wizard_state = REMAP_WAIT_PRESS;
static int target_index = 0;
static ButtonConfig tempConfigs[20];
static bool used_gpio[30] = {false};

// Boutons à ignorer
static bool is_skippable(int i) {
    const char* l = buttonConfigs[i].label;
    return (strcmp(l, "C") == 0 || strcmp(l, "Z") == 0 || strcmp(l, "CAP") == 0);
}

void start_remap_wizard() {
    memcpy(tempConfigs, buttonConfigs, sizeof(buttonConfigs));
    memset(used_gpio, 0, sizeof(used_gpio));
    wizard_state = REMAP_WAIT_PRESS;
    target_index = 0;
    while (target_index < 20 && is_skippable(target_index)) target_index++;
}

void process_remap_wizard_input() {
    if (currentPage != PAGE_REMAP_WIZARD || wizard_state != REMAP_WAIT_PRESS) return;

    for (int gpio = 0; gpio < 30; ++gpio) {
        if (!gpio_get(gpio)) { // bouton pressé
            if (used_gpio[gpio]) return; // déjà utilisé
            tempConfigs[target_index].gpio = gpio;
            used_gpio[gpio] = true;

            // Avancer au suivant
            do {
                target_index++;
            } while (target_index < 20 && is_skippable(target_index));

            if (target_index >= 20) {
                memcpy(buttonConfigs, tempConfigs, sizeof(buttonConfigs));
                apply_config_btn();
                wizard_state = REMAP_DONE;
                currentPage = PAGE_INPUT;
            }
            break;
        }
    }
}

RemapWizardState get_remap_wizard_state() {
    return wizard_state;
}

int get_remap_target_index() {
    return target_index;
}
