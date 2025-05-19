#ifndef HOTKEY_H
#define HOTKEY_H



#include "tusb.h"
#include "button_config.h"
#include "hardware/gpio.h"
#include "pico/bootrom.h"

void copy_buttons_from_profile();
bool is_button_pressed(const char* label);


void input_reboot_bootsel();
// void input_change_scocd_mode();
// void switch_mode_input();


void update_hotkeys();


#endif // HOTKEY_H