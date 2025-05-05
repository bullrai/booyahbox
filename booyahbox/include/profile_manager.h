#pragma once
#include <cstddef>
#include <cstdint>

// Nombre max. de profils qu'on réserve dans la Flash
static constexpr size_t MAX_PROFILES    = 10;
static constexpr size_t PROFILE_SIZE    = 64;     // octets, sizeof(Profile)
static constexpr uint32_t PROFILES_BASE = 0x100000; // adresse Flash où commencent les profils

// Ta struct binaire, fixée à PROFILE_SIZE
struct Profile {
    uint8_t  socd;               // 1
    uint8_t  mapping[16];        // 16
    uint8_t  macros[8];          // 8
    uint16_t latency_ms;         // 2
    uint8_t  reserved[32];       // padding
    uint32_t crc;                // 4
};

// Statuts de retour
enum class PM_Status {
    OK,
    CRC_ERROR,
    FLASH_ERROR,
    INVALID_INDEX
};

namespace ProfileManager {

    // Initialise le module (vérifie la validité CRC de chaque slot)
    void init();

    // Lit le profil i dans `out`; retourne OK ou CRC_ERROR/INVALID_INDEX
    PM_Status load(uint8_t index, Profile &out);

    // Sauvegarde `p` dans le slot index (efface+programme); retourne OK ou FLASH_ERROR
    PM_Status save(uint8_t index, const Profile &p);

    // Efface le slot (le marque comme vide, en mettant magic=0xFFFFFFFF)
    PM_Status erase(uint8_t index);

    // Applique `p` sur tes variables globales et lance les callbacks nécessaires
    void apply(const Profile &p);

    // Retourne le nombre de slots valides et remplit `indices[0..count-1]`
    size_t list_valid(uint8_t indices[MAX_PROFILES]);
}
