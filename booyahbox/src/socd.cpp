#include "socd.h"
#include "config.h"





// Définition de la variable globale pour la stratégie SOCD actuelle
SOCDStrategy current_socd_strategy = PROFILE.socd_strategy;

// Définition des fonctions pour chaque stratégie
int8_t neutral_strategy(bool neg, bool pos) {
    if (neg && pos) return 0;
    if (neg) return -127;
    if (pos) return 127;
    return 0;
}

int8_t up_priority_strategy(bool neg, bool pos, bool y) {
    if (neg && pos && y) return -127; // Si Y est actif, on renvoie 0
    if (neg && pos) return 0;
    if (neg) return -127;
    if (pos) return 127;
    return 0;
}

int8_t first_input_priority_strategy(bool neg, bool pos, int8_t &first_dir) {
    // Cas de relâchement total
    if (!neg && !pos) {
        first_dir = 0;
        return 0;
    }

    // Nouveau pressage
    if (neg && !pos) {
        first_dir = -127;
    } else if (pos && !neg) {
        first_dir = 127;
    }

    // Les deux sont pressés, on garde le dernier
    return first_dir;
}



int8_t last_input_priority_strategy(bool neg, bool pos, int8_t &first_dir) {
    // Cas de relâchement total
    if (!neg && !pos) {
        first_dir = 0;
        return 0;
    }

    // Nouveau pressage
    if (neg && !pos) {
        first_dir = -127;
    } else if (pos && !neg) {
        first_dir = 127;
    }

    if (neg && pos) {
        // Si les deux sont pressés, on garde le dernier pressé
        return -first_dir;
    }
    // Les deux sont pressés, on garde le dernier
    return first_dir;
}


// Fonction générale pour appliquer le SOCD cleaning sur un axe
int8_t apply_socd_cleaning(bool neg, bool pos, bool &first_input, int8_t &first_dir, bool y) {
    switch (current_socd_strategy) {
        case NEUTRAL:
            return neutral_strategy(neg, pos);
        case UP_PRIORITY:
            return up_priority_strategy(neg, pos, y);
        case FIRST_INPUT_PRIORITY:
            return first_input_priority_strategy(neg, pos, first_dir);
        case LAST_INPUT_PRIORITY:
            return last_input_priority_strategy(neg, pos, first_dir);
    }
    return 0; // Valeur par défaut si aucune stratégie ne correspond

}


// Fonction pour changer la stratégie SOCD
void change_socd_strategy() {
    static uint8_t strategy_index = 0;
    strategy_index = (strategy_index + 1) % 4;
    PROFILE.socd_strategy = static_cast<SOCDStrategy>(strategy_index);
    current_socd_strategy = PROFILE.socd_strategy;
    
}

// Fonction pour appliquer la stratégie SOCD
void apply_socd_neutral() {
    current_socd_strategy = NEUTRAL;
}
void apply_socd_up_priority() {
    current_socd_strategy = UP_PRIORITY;
}
void apply_socd_first_input_priority() {
    current_socd_strategy = FIRST_INPUT_PRIORITY;
}
void apply_socd_last_input_priority() {
    current_socd_strategy = LAST_INPUT_PRIORITY;
}
