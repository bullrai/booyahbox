// --- input.h ---
#pragma once
#include <stdint.h>
#include <stdbool.h>

extern bool use_dpad_mode;

void setup_pins();
uint16_t read_buttons();
int8_t read_axis(uint8_t pin_neg, uint8_t pin_pos, bool &first_input, int8_t &first_dir, bool y);
uint8_t read_hat(uint8_t gpio_up, uint8_t gpio_down, uint8_t gpio_left, uint8_t gpio_right);

void change_Dpad_mode();