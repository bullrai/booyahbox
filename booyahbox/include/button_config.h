// include/button_config.h
#pragma once
#include <stdint.h>

#define BUTTON_COUNT 16

extern uint8_t buttonPins[BUTTON_COUNT];
// Joystick via boutons (leverless style)
extern uint8_t X_LEFT_PIN     ;//2
extern uint8_t X_RIGHT_PIN    ;//3
extern uint8_t Y_UP_PIN       ;//4
extern uint8_t Y_DOWN_PIN     ;//5

uint8_t find_gpio_by_label(const char* label);
void apply_config_btn(); 
void setup_pins();