#pragma once

#include <stdint.h>
#include "tusb.h"

#ifdef __cplusplus
extern "C" {
#endif

const uint8_t* tud_descriptor_device_cb(void);
const uint8_t* tud_descriptor_configuration_cb(uint8_t index);
const uint8_t* tud_hid_descriptor_report_cb(uint8_t instance);
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid);
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, const uint8_t* buffer, uint16_t bufsize);
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen);

#ifdef __cplusplus
}
#endif
