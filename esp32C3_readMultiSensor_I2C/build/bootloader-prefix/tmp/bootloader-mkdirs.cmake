# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Documents/espressif/Espressif/frameworks/esp-idf-v4.4.4/components/bootloader/subproject"
  "D:/Documents/espressif/temp/esp32C3_readMultiSensor_I2C/build/bootloader"
  "D:/Documents/espressif/temp/esp32C3_readMultiSensor_I2C/build/bootloader-prefix"
  "D:/Documents/espressif/temp/esp32C3_readMultiSensor_I2C/build/bootloader-prefix/tmp"
  "D:/Documents/espressif/temp/esp32C3_readMultiSensor_I2C/build/bootloader-prefix/src/bootloader-stamp"
  "D:/Documents/espressif/temp/esp32C3_readMultiSensor_I2C/build/bootloader-prefix/src"
  "D:/Documents/espressif/temp/esp32C3_readMultiSensor_I2C/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Documents/espressif/temp/esp32C3_readMultiSensor_I2C/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
