cmake_minimum_required(VERSION 3.13)

# Définir le chemin vers le Pico SDK
set(PICO_SDK_PATH "$ENV{HOME}/pico/pico-sdk")

# Inclure et initialiser le SDK
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)
project(booyahbox C CXX ASM)
pico_sdk_init()

# Créer l'exécutable
add_executable(booyahbox
    src/main.cpp
)

# Activer HID et désactiver le CDC-USB stdio
target_compile_definitions(booyahbox PRIVATE
    CFG_TUD_HID=1
    PICO_STDIO_USB_ENABLE=0
)

# Include local pour tusb_config.h
target_include_directories(booyahbox PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Lier les bibliothèques Pico et TinyUSB
target_link_libraries(booyahbox
    pico_stdlib
    hardware_adc
    tinyusb_device
    tinyusb_board
)

# Générer le .uf2 pour BOOTSEL
pico_add_extra_outputs(booyahbox)