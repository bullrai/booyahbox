# BooyahBox Firmware

Firmware minimaliste et ultra-réactif pour stick arcade **100 % custom** basé sur **Raspberry Pi Pico**.  
Pensé pour une latence quasi nulle, un code direct, lisible et sans abstraction inutile.

---

## Objectifs

- Zéro latence perceptible
- Code C++ clair, structuré, modifiable facilement
- Aucun framework, aucune surcouche inutile
- Profils codés en dur (structure C++), avec option future pour binaire en flash
- SOCD cleaning activable ou non
- Mode joystick ou D-Pad sélectionnable
- Lecture des hotkeys sur le **deuxième core** RP2040 (isolation totale du gameplay)
- 100 % compatible PC / Linux / Steam

---

## Matériel

- **Microcontrôleur** : Raspberry Pi Pico (RP2040)
- **Entrées** : GPIO configurables pour boutons, directions, hotkeys
- **Sortie** : USB HID (Gamepad standard)
- **Stockage** : Profil codé en dur (binaire possible plus tard)
- **Affichage** : Aucun (prévu V2 si nécessaire)

---

## Arborescence

```
firmware/
├── main.cpp              # Boucle principale : lecture GPIO, envoi USB
├── config.cpp / .h       # Chargement du profil (struct C++)
├── input.cpp / .h        # Gestion directe des GPIO
├── usb_report.cpp / .h   # Format HID + envoi via TinyUSB
├── socd.cpp / .h         # Nettoyage des directions (optionnel)
├── hotkey.cpp / .h       # Lecture des hotkeys (exécuté sur le core 1)
├── CMakeLists.txt        # Build CMake pour Pico SDK
```

---

## Compilation

### Prérequis

- Pico SDK installé
- TinyUSB activé
- CMake + toolchain RP2040

### Compilation

```bash
mkdir build
cd build
cmake .. -G Ninja
ninja
```

---

## Flash du firmware

1. Maintenir le bouton BOOTSEL au branchement USB
2. Copier le `.uf2` généré dans la partition `RPI-RP2`
3. Le Pico redémarre automatiquement avec le firmware

---

## Structure d'un profil (en C++)

```cpp
struct Profile {
    uint8_t pin_map[NUM_INPUTS];     // GPIO utilisés
    uint16_t button_map[NUM_INPUTS]; // Masques HID associés
    bool use_dpad_mode;              // D-Pad ou joystick
    bool socd_enabled;               // Nettoyage des directions
};
```

Le profil est **compilé en dur** dans le firmware. Une future version pourra permettre un stockage en binaire dans la flash (optionnel).

---

## Philosophie du projet

- **Philosophie** : Carmackien
- **Moins, c'est mieux** : tout est optimisé pour la simplicité, la lisibilité, la performance.
- **Pas de magie** : tout comportement est traçable et compréhensible en lisant le code.
- **Pas d'abstraction inutile** : le matériel est manipulé directement.
- **Multicoeur maîtrisé** : un coeur = une responsabilité claire (core 0 = gameplay, core 1 = hotkeys)

---

## Auteurs

**Booyah Lab** — Matériel et firmware par des makers pour les joueurs exigeants.





## Optimisation suggérées

| Amélioration                          | Pourquoi le faire                                  | Complexité |     |
| ------------------------------------- | -------------------------------------------------- | ---------- | --- |
| Watchdog                              | Anti-freeze, plus de fiabilité                     | Faible     |     |
| Protection inter-core                 | Évite des bugs aléatoires difficiles à traquer     | Moyenne    |     |
| Centralisation HID                    | Maintenance plus simple                            | Faible     |     |
| `constexpr` dans `Profile`            | Compile-time safety, erreurs visibles à la compile | Moyenne    |     |
| Logger simple ou compteur d’anomalies | Aide au debug (e.g. bouton bloqué)                 | Faible     |     |
| Mesure temps boucle                   | Pour traquer les latences                          | Faible     |     |
