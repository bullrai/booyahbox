#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

// --- Microcontrôleur et mode USB ---
#define CFG_TUSB_MCU           OPT_MCU_RP2040
#define CFG_TUSB_RHPORT0_MODE  OPT_MODE_DEVICE

// --- Taille de l’endpoint 0 ---
#define CFG_TUD_ENDPOINT0_SIZE 64

// --- Classes USB Device ---
#define CFG_TUD_HID    1    // HID pour gamepad
#define CFG_TUD_CDC    0    // Pas de CDC (série virtuelle)
#define CFG_TUD_MSC    0    // Pas de MSC
#define CFG_TUD_MIDI   0    // Pas de MIDI
#define CFG_TUD_VENDOR 0    // Pas de vendor

// --- Buffer HID ---
#define CFG_TUD_HID_EP_BUFSIZE 64

#endif /* _TUSB_CONFIG_H_ */


