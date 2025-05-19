// remap.h
#pragma once

// Appelé depuis la boucle principale pour gérer les entrées
void process_remap_menu_input();

// Utilisé pour l'affichage OLED
enum RemapState {
    WAIT_BUTTON,
    CHOOSE_FUNCTION
};

RemapState get_remap_state();
int get_remap_index();       // Index du bouton physique en cours de remap
int get_remap_selection();   // Index de la fonction HID sélectionnée
