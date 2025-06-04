# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

add_compile_options(-fdiagnostics-color=always)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# set(ESP_IDF_LIB_PATH ${CMAKE_CURRENT_LIST_DIR}/components/extra/esp-idf-lib/components)
# set(EXTRA_COMPONENT_DIRS ${ESP_IDF_LIB_PATH}/i2cdev)

# Adds necessary definitions for compiling it using Serial symbol attached to the HW USB CDC port
list(APPEND compile_definitions "ARDUINO_USB_CDC_ON_BOOT=1")
list(APPEND compile_definitions "ARDUINO_USB_MODE=1")

idf_build_set_property(MINIMAL_BUILD ON)
project(mobilenet)
