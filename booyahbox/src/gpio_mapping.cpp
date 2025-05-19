// Nouveau fichier : gpio_mapping.cpp

#include "gpio_mapping.h"
#include "hardware/gpio.h"


GpioMapping gpioMappings[MAX_GPIO_MAPPINGS];
int gpioMappingCount = 0;

void map_gpio_to_hid(uint8_t gpio, uint8_t hid_bit) {
    if (gpioMappingCount >= MAX_GPIO_MAPPINGS) return;
    gpioMappings[gpioMappingCount++] = { gpio, hid_bit };
}

void setup_input_pins() {
    for (int i = 0; i < gpioMappingCount; ++i) {
        gpio_init(gpioMappings[i].gpio);
        gpio_set_dir(gpioMappings[i].gpio, GPIO_IN);
        gpio_pull_up(gpioMappings[i].gpio);
    }
}

uint16_t read_buttons_from_mappings() {
    uint16_t buttons = 0;
    for (int i = 0; i < gpioMappingCount; ++i) {
        if (!gpio_get(gpioMappings[i].gpio)) {
            buttons |= (1 << gpioMappings[i].hid_bit);
        }
    }
    return buttons;
}

void clear_gpio_mappings() {
    gpioMappingCount = 0;
}
