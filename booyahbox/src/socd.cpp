#include "socd.h"



// Définition de la variable globale pour la stratégie SOCD actuelle
SOCDStrategy current_socd_strategy = NEUTRAL;

// Définition des fonctions pour chaque stratégie
int8_t neutral_strategy(bool neg, bool pos) {
    if (neg && pos) return 0;
    if (neg) return -127;
    if (pos) return 127;
    return 0;
}

int8_t up_priority_strategy(bool neg, bool pos) {
    if (neg && pos) return 127;
    if (neg) return -127;
    if (pos) return 127;
    return 0;
}

int8_t first_input_priority_strategy(bool neg, bool pos, bool &first_input, int8_t &last_state) {
    if (!neg && !pos) {
        first_input = true;
        last_state = 0;
        return 0;
    }
    if (first_input) {
        last_state = (neg ? -127 : 127);
        first_input = false;
    }
    return last_state;
}

int8_t last_input_priority_strategy(bool neg, bool pos, int8_t &last_state) {
    if (neg && !last_state) last_state = -127;
    if (pos && last_state == neg) last_state = 127;
    if (neg && pos) return last_state;
    if (neg) return -127;
    if (pos) return 127;
    last_state = 0;
    return 0;
}

// Fonction générale pour appliquer le SOCD cleaning sur un axe
int8_t apply_socd_cleaning(bool neg, bool pos, bool &first_input, int8_t &last_state) {
    using StrategyFunc = std::function<int8_t(bool, bool, bool&, int8_t&)>;

    // Tableau de fonctions pour chaque stratégie
    static const StrategyFunc strategies[] = {
        [](bool neg, bool pos, bool &first_input, int8_t &last_state) { return neutral_strategy(neg, pos); },
        [](bool neg, bool pos, bool &first_input, int8_t &last_state) { return up_priority_strategy(neg, pos); },
        first_input_priority_strategy,
        [](bool neg, bool pos, bool &first_input, int8_t &last_state) { return last_input_priority_strategy(neg, pos, last_state); }
    };

    // Appeler la fonction correspondant à la stratégie actuelle
    return strategies[current_socd_strategy](neg, pos, first_input, last_state);
}


// Fonction pour changer la stratégie SOCD
void change_socd_strategy() {
    static uint8_t strategy_index = 0;
    strategy_index = (strategy_index + 1) % 4;
    current_socd_strategy = static_cast<SOCDStrategy>(strategy_index);
    
}