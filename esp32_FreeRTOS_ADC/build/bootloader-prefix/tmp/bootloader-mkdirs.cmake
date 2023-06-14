# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Documents/espressif/Espressif/frameworks/esp-idf-v4.4.4/components/bootloader/subproject"
  "D:/Documents/ESP32C3_Project/ESP-IDF_DatHoang_Project/ESP-IDF_DatHoang_Project/esp32_FreeRTOS_ADC/build/bootloader"
  "D:/Documents/ESP32C3_Project/ESP-IDF_DatHoang_Project/ESP-IDF_DatHoang_Project/esp32_FreeRTOS_ADC/build/bootloader-prefix"
  "D:/Documents/ESP32C3_Project/ESP-IDF_DatHoang_Project/ESP-IDF_DatHoang_Project/esp32_FreeRTOS_ADC/build/bootloader-prefix/tmp"
  "D:/Documents/ESP32C3_Project/ESP-IDF_DatHoang_Project/ESP-IDF_DatHoang_Project/esp32_FreeRTOS_ADC/build/bootloader-prefix/src/bootloader-stamp"
  "D:/Documents/ESP32C3_Project/ESP-IDF_DatHoang_Project/ESP-IDF_DatHoang_Project/esp32_FreeRTOS_ADC/build/bootloader-prefix/src"
  "D:/Documents/ESP32C3_Project/ESP-IDF_DatHoang_Project/ESP-IDF_DatHoang_Project/esp32_FreeRTOS_ADC/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Documents/ESP32C3_Project/ESP-IDF_DatHoang_Project/ESP-IDF_DatHoang_Project/esp32_FreeRTOS_ADC/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
