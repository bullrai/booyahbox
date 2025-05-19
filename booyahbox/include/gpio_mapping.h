#pragma once
#include <stdint.h>

void map_gpio_to_hid(uint8_t gpio, uint8_t hid_bit);
void setup_input_pins();
uint16_t read_buttons_from_mappings();
void clear_gpio_mappings();