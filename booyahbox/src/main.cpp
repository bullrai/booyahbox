// --- main.cpp ---
#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb_config.h"
#include "pico/multicore.h"

#include "config.h"
#include "button_config.h"
#include "input.h"
#include "socd.h"
#include "hotkey.h"

struct __attribute__((packed)) GamepadReport {
    int8_t x;
    int8_t y;
    uint16_t buttons;  // 16 boutons 
};

// cœur 1 : gestion écran
extern "C" void core1_entry() {
    copy_buttons_from_profile();
    while (true) {
        // input_reboot_bootsel();
        update_hotkeys();
        sleep_ms(100);  // change toutes les 500 ms
    }
}

int main() {
    stdio_init_all();
    board_init();
    tusb_init();
    
    apply_config_btn();

    setup_pins();

    multicore_launch_core1(core1_entry);
    
    static bool first_input_x = true, first_input_y = true;
    static int8_t first_dir_x = 0, first_dir_y = 0;
    while (true) {
        tud_task();
        
        // input_reboot_bootsel();

        
        GamepadReport report = {};

        // Lecture des boutons
        
        // Lecture des axes
        
        report.x = read_axis(X_LEFT_PIN, X_RIGHT_PIN, first_input_x, first_dir_x, false);
        report.y = read_axis(Y_UP_PIN, Y_DOWN_PIN, first_input_y, first_dir_y, true);
        
        report.buttons = read_buttons();
        // Lecture du D-Pad si en mode D-Pad
        // if (PROFILE.use_dpad_mode) {
        //     report.hat = read_hat(Y_UP_PIN, Y_DOWN_PIN, X_LEFT_PIN, X_RIGHT_PIN);
        // } else {
        //     report.hat = 0; // neutre
        // }

        if (tud_hid_ready()) {
            tud_hid_report(0, &report, sizeof(report));
        }

        sleep_ms(5);
    }
    return 0;
}