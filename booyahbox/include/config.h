// --- config.h ---
#pragma once
#include <stdint.h>
#include "socd.h"

const uint8_t GPIO_COUNT = 28;

struct Profile {
    const char* name;
    struct {
        uint8_t gpio;
        const char* label;
    } buttons[GPIO_COUNT];

    bool use_dpad_mode;
    bool socd_enabled;
    SOCDStrategy socd_strategy;
};

extern Profile PROFILE;