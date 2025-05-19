// include/display.h
#pragma once
#include <cstdint>
#include "stdlib.h"


// Broches I²C
static constexpr uint8_t SDA_PIN   = 0;
static constexpr uint8_t SCL_PIN   = 1;
// Adresse I²C
static constexpr uint8_t OLED_ADDR = 0x3C;

// Pages du menu
enum Page {
    PAGE_SPLASH = 0,
    PAGE_SOCD,
    PAGE_INPUT,
    PAGE_REMAPPING,
    PAGE_REMAP_WIZARD,
    PAGE_COUNT    // toujours placer en dernier
};

// Modes d’utilisation
enum Mode {
    MODE_GAME = 0,
    MODE_CONFIG
};

// Variables partagées entre Core0 et Core1
extern volatile Mode    currentMode;
extern volatile int     currentPage;    // [0..PAGE_COUNT-1]

// Prototypes pour Core 1
void check_page_nav();
void init_display();
void display_task();
