// BouyahBox Firmware v0.1 — Gamepad HID 20 boutons

#include "tusb_config.h"
#include "tusb.h"
#include "pico/stdlib.h"
#include "bsp/board.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pins.h"



// --- Descripteur HID Gamepad 20 boutons (littéral) ---
static const uint8_t hid_report_desc[] = {
    0x05, 0x01,       // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,       // USAGE (Game Pad)
    0xA1, 0x01,       // COLLECTION (Application)
    0x05, 0x09,       //   USAGE_PAGE (Button)
    0x19, 0x01,       //   USAGE_MINIMUM (Button 1)
    0x29, 0x14,       //   USAGE_MAXIMUM (Button 20)
    0x15, 0x00,       //   LOGICAL_MINIMUM (0)
    0x25, 0x01,       //   LOGICAL_MAXIMUM (1)
    0x95, 0x14,       //   REPORT_COUNT (20)
    0x75, 0x01,       //   REPORT_SIZE (1)
    0x81, 0x02,       //   INPUT (Data,Var,Abs)
    0x95, 0x01,       //   REPORT_COUNT (1) Padding
    0x75, 0x0B,       //   REPORT_SIZE (11)
    0x81, 0x03,       //   INPUT (Const,Var,Abs)
    0xC0              // END_COLLECTION
};

// --- Device Descriptor ---
tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(desc_device),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = TUSB_CLASS_MISC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = 0xCafe,
    .idProduct          = 0x4000,
    .bcdDevice          = 0x0100,
    .iManufacturer      = 1,
    .iProduct           = 2,
    .iSerialNumber      = 3,
    .bNumConfigurations = 1
};

// Callback: Device descriptor
extern "C" uint8_t const* tud_descriptor_device_cb(void) {
    return (uint8_t const*)&desc_device;
}

// Configuration Descriptor
enum { ITF_NUM_HID = 0, ITF_NUM_TOTAL };
#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)
static uint8_t const desc_configuration[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0, 50),
    TUD_HID_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE,
                       sizeof(hid_report_desc), 0x81,
                       CFG_TUD_HID_EP_BUFSIZE, 10)
};

extern "C" uint8_t const* tud_descriptor_configuration_cb(uint8_t) {
    return desc_configuration;
}

// Callback: HID report descriptor
extern "C" uint8_t const* tud_hid_descriptor_report_cb(uint8_t) {
    return hid_report_desc;
}

// --- Initialisation des broches ---
void setup_pins() {
    // DPad setup
    gpio_init(DPAD_UP_PIN);
    gpio_set_dir(DPAD_UP_PIN, GPIO_IN);
    gpio_pull_up(DPAD_UP_PIN);

    gpio_init(DPAD_DOWN_PIN);
    gpio_set_dir(DPAD_DOWN_PIN, GPIO_IN);
    gpio_pull_up(DPAD_DOWN_PIN);

    gpio_init(DPAD_LEFT_PIN);
    gpio_set_dir(DPAD_LEFT_PIN, GPIO_IN);
    gpio_pull_up(DPAD_LEFT_PIN);

    gpio_init(DPAD_RIGHT_PIN);
    gpio_set_dir(DPAD_RIGHT_PIN, GPIO_IN);
    gpio_pull_up(DPAD_RIGHT_PIN);

    // Buttons setup
    for (int i = 0; i < BUTTON_COUNT; i++) {
        gpio_init(buttonPins[i]);
        gpio_set_dir(buttonPins[i], GPIO_IN);
        gpio_pull_up(buttonPins[i]);
    }
}

// --- Lecture des boutons ---
uint16_t read_buttons() {
    uint16_t buttons = 0;

    for (int i = 0; i < BUTTON_COUNT; i++) {
        if (!gpio_get(buttonPins[i])) {
            buttons |= (1 << i);
        }
    }

    return buttons;
}

// --- Lecture du DPad ---
uint8_t read_dpad() {
    bool up    = !gpio_get(DPAD_UP_PIN);
    bool down  = !gpio_get(DPAD_DOWN_PIN);
    bool left  = !gpio_get(DPAD_LEFT_PIN);
    bool right = !gpio_get(DPAD_RIGHT_PIN);

    if (up && left)  return 7; // Haut-Gauche
    if (up && right) return 1; // Haut-Droite
    if (down && left) return 5; // Bas-Gauche
    if (down && right) return 3; // Bas-Droite
    if (up)    return 0; // Haut
    if (right) return 2; // Droite
    if (down)  return 4; // Bas
    if (left)  return 6; // Gauche

    return 8; // Centre (aucune direction)
}

// --- Main program ---
int main() {
    board_init();    // Initialise la carte Pico
    tusb_init();     // Initialise la stack USB TinyUSB
    setup_pins();    // Configure toutes les entrées GPIO

    while (true) {
        tud_task();  // TinyUSB Device Task obligatoire

        uint16_t buttons = read_buttons(); // Lire tous les boutons
        uint8_t dpad = read_dpad();         // Lire le DPad

        if (tud_hid_ready()) {
            uint8_t report[4] = {0};

            report[0] = buttons & 0xFF;         // 8 premiers boutons
            report[1] = (buttons >> 8) & 0xFF;  // 6 suivants (max 14 boutons ici)
            report[2] = dpad;                   // DPad value
            report[3] = 0;                      // Réservé (axes analogiques par ex.)

            tud_hid_report(0, report, sizeof(report));
        }

        sleep_ms(5); // Pause pour éviter de saturer l'USB
    }
}

// --- Callbacks stubs pour TinyUSB ---
extern "C" uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    // Pas de chaînes supportées
    return nullptr;
}

extern "C" void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    // SET_REPORT handler (non utilisé dans ce firmware)
}

extern "C" uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    // GET_REPORT handler (non utilisé)
    return 0;
}