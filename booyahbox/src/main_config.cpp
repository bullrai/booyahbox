#include "main_config.h"
#include "pico/stdlib.h"
#include "display.h"
#include "button_config.h"
#include "socd.h"
// #include "profile_manager.h"
// #include "remap.h"

// extern void process_socd_menu_input();
// extern void process_remap_menu_input();
// extern void display_task();

void main_config() {
    setup_pins();
    init_display();
    // ProfileManager::init();

    // while (true) {
    //     process_socd_menu_input();
    //     process_remap_menu_input();
    //     display_task();
    //     sleep_ms(16); // 60 FPS
    // }
}
