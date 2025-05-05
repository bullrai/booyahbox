// BouyahBox Firmware v0.1 — Gamepad HID 20 boutons

#include "tusb_config.h"
#include "tusb.h"
#include "pico/stdlib.h"
#include "bsp/board.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/bootrom.h"
#include "hardware/timer.h"
#include "pico/multicore.h"

// #include "profile_manager.h"
#include "pins.h"
#include "socd.h"
#include "hotkey.h"
#include "display.h"
#include "usb_descriptors.cpp"
#include "board_setup.cpp"

// uint8_t valid_slots[MAX_PROFILES];
// size_t  nb_profiles;

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



int8_t read_axis(uint pin_neg, uint pin_pos, bool first_input, int8_t last_state) {
    bool neg = !gpio_get(pin_neg);
    bool pos = !gpio_get(pin_pos);
    int8_t axis = apply_socd_cleaning(neg, pos, first_input, last_state);
    return axis;
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


//         ==========DIPLAY==========
// Variables partagées
volatile Mode currentMode = MODE_GAME;
volatile int  currentPage = PAGE_SPLASH;  // on démarre sur SOCD après splash
static bool combo_act = false;
static absolute_time_t combo_sta;
// Detection combo Start+Left+Right pour toggle MODE
static void check_mode_toggle() {
    bool btn1 = !gpio_get(BUTTON_START_PIN);
    bool btn2 = !gpio_get(BUTTON_X_PIN);
    bool btn3 = !gpio_get(BUTTON_LS_PIN);
    
    if (btn1 && btn2 && btn3) {
        if (!combo_act) {
            combo_act = true;
            combo_sta  = get_absolute_time();
            
        } else {
            // déjà en mode “combo”, on vérifie la durée
            int64_t dt_us = absolute_time_diff_us(combo_sta, get_absolute_time());
            if (dt_us >= 3 * 1000000) {
                // 3 secondes écoulées → reboot en BOOTSEL
                currentMode = (currentMode==MODE_GAME?MODE_CONFIG:MODE_GAME);
                combo_sta = get_absolute_time();
                combo_act = false;
                // note : le code ne revient jamais ici, la puce redémarre
            }
        }
    } else {
        // relâchement de l’un des deux boutons : on réarme
        combo_act = false;
    }
}

// Navigation pages si Config
static void check_page_nav() {
    if (currentMode != MODE_CONFIG) return;
    bool left  = !gpio_get(X_LEFT_PIN);
    bool right = !gpio_get(X_RIGHT_PIN);
    static bool nav_active = false;
    if ((left||right) && !nav_active) {
        nav_active = true;
        if (left) {
            currentPage = (currentPage + PAGE_COUNT - 1) % PAGE_COUNT;
        } else {
            currentPage = (currentPage + 1) % PAGE_COUNT;
        }
    } else if (!left && !right) {
        nav_active = false;
    }
}
// cœur 1 : gestion écran
extern "C" void core1_entry() {
    init_display();
    while (true) {
        display_task();
        sleep_ms(50);  // change toutes les 500 ms
    }
}

static bool up_act = false;
static bool dn_act = false;
static bool ok_act = false;

void process_socd_menu_input() {

    if (currentMode != MODE_CONFIG || currentPage != PAGE_SOCD) return;

    bool up      = !gpio_get(Y_UP_PIN);
    bool down    = !gpio_get(Y_DOWN_PIN);
    bool confirm = !gpio_get(BUTTON_A_PIN);

    // ↑
    if (up && !up_act) {
        up_act = true;
        if (current_socd_strategy > SOCDStrategy::NEUTRAL) {
            current_socd_strategy = SOCDStrategy((uint8_t)current_socd_strategy - 1);
        }
    } else if (!up) {
        up_act = false;
    }
    // ↓
    if (down && !dn_act) {
        dn_act = true;
        if (current_socd_strategy < SOCDStrategy::LAST_INPUT_PRIORITY) {
            current_socd_strategy = SOCDStrategy((uint8_t)current_socd_strategy + 1);
        }
    } else if (!down) {
        dn_act = false;
    }
    // X (confirmation)
    if (confirm && !ok_act) {
        ok_act = true;
        current_socd_strategy = SOCDStrategy((uint8_t)current_socd_strategy);
        // persister immédiatement
        // save_config();                  // votre fonction qui écrit le JSON
        // apply_socd_strategy();          // applique la nouvelle stratégie tout de suite
    } else if (!confirm) {
        ok_act = false;
    }
}

// --- Main program ---
int main() {
    stdio_init_all();
    board_init();
    tusb_init();
    
    // ProfileManager::init();
    // nb_profiles = ProfileManager::list_valid(valid_slots);
    // Profile p;
    // if (ProfileManager::load(2, p) == PM_Status::OK) {
    //     ProfileManager::apply(p);
    // }

    setup_pins();
    
    multicore_launch_core1(core1_entry);
    
    // init_oled_display();


    static int8_t last_x = 0, last_y = 0; // Pour Last-Input Priority
    static bool first_input_x = true, first_input_y = true; // Pour First-Input Priority
    while (true) {
        tud_task();
        
        
        check_mode_toggle();
        check_page_nav();
        process_socd_menu_input();
        
        input_reboot_bootsel();
        // Vérifier la combinaison de touches pour changer la stratégie SOCD

        if (currentMode == MODE_GAME) {
            GamepadReport report = {};
            
            report.x = read_axis(X_LEFT_PIN, X_RIGHT_PIN, last_x, first_input_x); // -127 à 127
            
            report.y = read_axis(Y_UP_PIN, Y_DOWN_PIN, last_y, first_input_y);; // -127 à 127
            report.buttonss = read_buttons();
            
            // Appliquer le SOCD cleaning sur les axes x et y

            if (tud_hid_ready()) {
                tud_hid_report(0, &report, sizeof(report));
            }

        }
        sleep_ms(5);  // Délai pour éviter de surcharger le bus USB
    }
}


