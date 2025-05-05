cmake_minimum_required(VERSION 3.13)

# Définir le chemin vers le Pico SDK
set(PICO_SDK_PATH "$ENV{HOME}/pico/pico-sdk")

# Inclure et initialiser le SDK
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)
project(booyahbox C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()


add_subdirectory(lib/pico-ssd1306)

# Créer l'exécutable
add_executable(booyahbox
src/main.cpp
# src/profile_manager.cpp
src/socd.cpp
src/hotkey.cpp
src/display.cpp
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
    hardware_i2c
    pico_ssd1306
    hardware_flash
    hardware_adc
    hardware_gpio
    hardware_dma
    hardware_spi  
    tinyusb_device
    tinyusb_board
    pico_bootrom
    hardware_timer
    pico_multicore
)



# Générer le .uf2 pour BOOTSEL
pico_add_extra_outputs(booyahbox)