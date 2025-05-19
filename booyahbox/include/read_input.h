// include/read_input.h

#pragma once
#include <stdint.h>  // pour uint*_t
#include <stdbool.h> // pour bool



uint16_t read_buttons(void);

int8_t read_axis(uint8_t pin_neg, uint8_t pin_pos, bool &first_input, int8_t &last_state , bool y_axis);

uint8_t read_hat(uint8_t gpio_up, uint8_t gpio_down, uint8_t gpio_left, uint8_t gpio_right);
