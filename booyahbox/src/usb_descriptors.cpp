#include "usb_descriptors.h"
#include "tusb.h"
// #include "pins.h" 


// --- Device Descriptor ---
tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(desc_device),
    .bDescriptorType    = TUSB_DESC_DEVICE,            // Toujours 0x01 pour un device descriptor
    .bcdUSB             = 0x0200,                      // Version USB 2.00
    .bDeviceClass       = TUSB_CLASS_MISC,             // Classe “Miscellaneous” (IAD)  
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,       
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,      // Taille max du endpoint 0 (control)
    .idVendor           = 0xCafe,                      // Vendor ID (à changer pour un VID officiel)
    .idProduct          = 0x333,                       // Product ID (ta sélection)
    .bcdDevice          = 0x0100,                      // Device release number (v1.00)
    .iManufacturer      = 1,                           // Index de la string “Manufacturer”
    .iProduct           = 2,                           // Index de la string “Product”
    .iSerialNumber      = 3,                           // Index de la string “Serial Number”
    .bNumConfigurations = 1,                           // Nombre de configurations (généralement 1)
};

// --- Descripteur HID Gamepad 20 boutons (littéral) ---
static const uint8_t hid_report_desc[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x05,        // Usage (Game Pad)
    0xA1, 0x01,        // Collection (Application)
    
    // Joystick axes: LX, LY, RX, RY
    0x05, 0x01,        //   Usage Page (Generic Desktop)
    0x09, 0x30,        //   Usage (X)
    0x09, 0x31,        //   Usage (Y)
    0x15, 0x81,        //   Logical Minimum (-127)
    0x25, 0x7F,        //   Logical Maximum (127)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x02,        //   Report Count (4)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)

    // 16 boutons
    0x05, 0x09,        //   Usage Page (Button)
    0x19, 0x01,        //   Usage Minimum (1)
    0x29, 0x10,        //   Usage Maximum (16)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x95, 0x10,        //   Report Count (16)
    0x75, 0x01,        //   Report Size (1)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)

    
    0xC0               // End Collection
};


// Indices pour les string descriptors
enum {
    STRID_LANGID = 0,     // toujours 0 : liste des langues supportées
    STRID_MANUFACTURER,   // 1 ← iManufacturer
    STRID_PRODUCT,        // 2 ← iProduct
    STRID_SERIAL          // 3 ← iSerialNumber
  };
  
// Les chaînes elles-mêmes (UTF-16LE attendues par USB)
static char const* string_desc[] = {
(const char[]){ 0x09, 0x04 },  // STRID_LANGID : anglais US
"BooyahBOX",                    // STRID_MANUFACTURER
"Luna",                 // STRID_PRODUCT
"SN2025-0001"                   // STRID_SERIAL
};


// Callback: Device descriptor
extern "C" uint8_t const* tud_descriptor_device_cb(void) {
    return (uint8_t const*)&desc_device;
}

// Configuration Descriptor
enum { ITF_NUM_HID = 0, ITF_NUM_TOTAL };
#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)
static uint8_t const desc_configuration[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0, 100),
    TUD_HID_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE,
                       sizeof(hid_report_desc), 0x81,
                       CFG_TUD_HID_EP_BUFSIZE, 10)
};

extern "C" uint8_t const* tud_descriptor_configuration_cb(uint8_t) {
    return desc_configuration;
}

// Callback: HID report descriptor
extern "C" uint8_t const* tud_hid_descriptor_report_cb(uint8_t instance) {
    (void) instance; // for multiple instance support
    return hid_report_desc;
}


extern "C" uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
  static uint16_t desc_buf[32];
  uint8_t  chr_count;

  // LangID ?
  if ( index == 0 )
  {
    // bLength = 4, bDescriptorType = STRING, wLANGID[0] = 0x0409
    desc_buf[0] = (TUSB_DESC_STRING << 8) | 4;
    desc_buf[1] = 0x0409;
    return desc_buf;
  }

  // Récupérer la chaîne C pour cet index
  if ( index >= sizeof(string_desc)/sizeof(string_desc[0]) ) return nullptr;
  const char* str = string_desc[index];

  // Nombre de caractères ASCII (max 31 pour ne pas déborder desc_buf)
  chr_count = strlen(str);
  if ( chr_count > 31 ) chr_count = 31;

  // bLength = (2 bytes per char + 2 bytes header)
  desc_buf[0] = (TUSB_DESC_STRING << 8) | (2*chr_count + 2);

  // Copier chaque caractère ASCII dans wCHAR (UTF-16LE)
  for(uint8_t i = 0; i < chr_count; i++)
  {
    desc_buf[1 + i] = str[i];
  }

  return desc_buf;
}

extern "C" void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    // SET_REPORT handler (non utilisé dans ce firmware)
}

extern "C" uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    // GET_REPORT handler (non utilisé)
    return 0;
}