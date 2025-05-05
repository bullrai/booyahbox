#pragma once
#include <cstdint>

// Un item de menu : étiquette + valeur (ex. enum SOCDStrategy)
typedef struct {
    const char* label;
    int32_t     value;
} MenuItem;

// Un menu dynamique : tableau d’items + count + sélection courante
typedef struct {
    const MenuItem* items;
    uint8_t         count;
    uint8_t         selected;
} Menu;

// Initialise le menu (index 0 sélectionné)
static inline void menu_init(Menu* m, const MenuItem* items, uint8_t count, int32_t initial_value) {
    m->items    = items;
    m->count    = count;
    // si initial_value correspond à un item, on l’aligne sur selected
    m->selected = 0;
    for (uint8_t i = 0; i < count; i++) {
        if (items[i].value == initial_value) {
            m->selected = i;
            break;
        }
    }
}

// Déplace la sélection vers le haut (si possible)
static inline void menu_up(Menu* m) {
    if (m->selected > 0) m->selected--;
}

// Déplace la sélection vers le bas (si possible)
static inline void menu_down(Menu* m) {
    if (m->selected + 1 < m->count) m->selected++;
}

// Récupère la valeur de l’item sélectionné
static inline int32_t menu_get_value(const Menu* m) {
    return m->items[m->selected].value;
}
