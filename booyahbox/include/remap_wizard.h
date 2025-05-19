// remap_wizard.h
#pragma once

// États du remap
enum RemapWizardState {
    REMAP_IDLE,
    REMAP_WAIT_PRESS,
    REMAP_DONE
};

// Démarre le remapping séquentiel
void start_remap_wizard();

// À appeler dans la boucle principale
void process_remap_wizard_input();

// Pour affichage OLED
RemapWizardState get_remap_wizard_state();
int get_remap_target_index();
