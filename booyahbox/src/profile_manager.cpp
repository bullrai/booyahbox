#include "profile_manager.h"
#include "hardware/flash.h"
#include <cstring>
#include "socd.h"

// CRC32 simple (tu peux en choper une small-footprint)
extern uint32_t crc32(const uint8_t *data, size_t len);

// Helper pour l’adresse Flash d’un slot
static inline uint32_t slot_addr(uint8_t i) {
    return PROFILES_BASE + i * PROFILE_SIZE;
}

void ProfileManager::init() {
    // Rien d’obligatoire ici : on n’a pas besoin de cache en RAM
}

PM_Status ProfileManager::load(uint8_t idx, Profile &out) {
    if (idx >= MAX_PROFILES) return PM_Status::INVALID_INDEX;
    uint32_t addr = slot_addr(idx);
    // Lecture directe via XIP
    memcpy(&out, (void*)(XIP_BASE + addr), PROFILE_SIZE);
    // Vérifie le CRC
    uint32_t crc = out.crc;
    out.crc = 0;
    if (crc32((uint8_t*)&out, PROFILE_SIZE) != crc) {
        return PM_Status::CRC_ERROR;
    }
    out.crc = crc;
    return PM_Status::OK;
}

PM_Status ProfileManager::save(uint8_t idx, const Profile &p) {
    if (idx >= MAX_PROFILES) return PM_Status::INVALID_INDEX;
    uint32_t addr = slot_addr(idx);
    // Prépare un buffer RAM modifiable (pour recalcul CRC)
    Profile buf = p;
    buf.crc = 0;
    buf.crc = crc32((uint8_t*)&buf, PROFILE_SIZE);
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(addr, FLASH_SECTOR_SIZE);
    int res = flash_range_program(addr, (uint8_t*)&buf, PROFILE_SIZE);
    restore_interrupts(ints);
    return res == 0 ? PM_Status::OK : PM_Status::FLASH_ERROR;
}

PM_Status ProfileManager::erase(uint8_t idx) {
    if (idx >= MAX_PROFILES) return PM_Status::INVALID_INDEX;
    Profile empty{};
    // On marque magic=0xFF dans le champ crc => invalid CRC
    empty.crc = 0xFFFFFFFF;
    return save(idx, empty);
}

size_t ProfileManager::list_valid(uint8_t out[MAX_PROFILES]) {
    size_t count = 0;
    for (uint8_t i = 0; i < MAX_PROFILES; i++) {
        Profile tmp;
        if (load(i, tmp) == PM_Status::OK) {
            out[count++] = i;
        }
    }
    return count;
}

void ProfileManager::apply(const Profile &p) {
    extern SOCDStrategy current_socd_strategy;
    extern uint8_t      mapping[16];
    extern uint8_t      macros[8];
    extern uint16_t     current_latency_ms;

    current_socd_strategy = (SOCDStrategy)p.socd;
    memcpy(mapping, p.mapping, sizeof(p.mapping));
    memcpy(macros,   p.macros,   sizeof(p.macros));
    current_latency_ms = p.latency_ms;

    // apply_socd_strategy();
    // … autre callback si besoin …
}
