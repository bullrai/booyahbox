#ifndef SOCD_H
#define SOCD_H

#include <stdint.h>
#include <functional>

// Énumération des stratégies SOCD
enum SOCDStrategy {
    NEUTRAL,
    UP_PRIORITY,
    FIRST_INPUT_PRIORITY,
    LAST_INPUT_PRIORITY
};

// Variable globale pour la stratégie SOCD actuelle
extern SOCDStrategy current_socd_strategy;
// --- Constantes pour le menu SOCD ---
static constexpr uint8_t SOCD_COUNT = 4;
static const char* socd_labels[SOCD_COUNT] = {
    "Neutral",
    "Up Priority",
    "First-Input Priority",
    "Last-Input Priority"
};
// Déclaration des fonctions pour chaque stratégie
int8_t neutral_strategy(bool neg, bool pos);
int8_t up_priority_strategy(bool neg, bool pos);
int8_t first_input_priority_strategy(bool neg, bool pos, bool &first_input, int8_t &last_state);
int8_t last_input_priority_strategy(bool neg, bool pos, int8_t &last_state);

// Fonction générale pour appliquer le SOCD cleaning sur un axe
int8_t apply_socd_cleaning(bool neg, bool pos, bool &first_input, int8_t &last_state);

void change_socd_strategy();

#endif // SOCD_H
