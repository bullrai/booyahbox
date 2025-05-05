#ifndef HOTKEY_H
#define HOTKEY_H



#include "tusb.h"
#include "pins.h"
#include "hardware/gpio.h"
#include "pico/bootrom.h"



void input_reboot_bootsel();
void input_change_scocd_mode();
// void switch_mode_input();


#endif // HOTKEY_H