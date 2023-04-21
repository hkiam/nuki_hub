#!/bin/sh

if [[ "$(docker images -q arduinosdk 2> /dev/null)" == "" ]]; then
  echo "build docker image"
  docker build -t arduinosdk .
fi

if [[ ! -d "Arduino-CMake-Toolchain" ]] ; then
    git clone --recurse-submodules https://github.com/technyon/Arduino-CMake-Toolchain.git
fi

if [[ ! -d "build" ]] ; then
    mkdir build
    echo "# Espressif ESP32 Partition Table" > ./build/partitions.csv
    echo "# Name, Type, SubType, Offset, Size, Flags" >> ./build/partitions.csv
    echo "nvs,      data, nvs,     0x9000,  0x5000," >> ./build/partitions.csv
    echo "otadata,  data, ota,     0xe000,  0x2000," >> ./build/partitions.csv
    echo "app0,     app,  ota_0,   0x10000, 0x1E0000," >> ./build/partitions.csv
    echo "app1,     app,  ota_1,   0x1F0000,0x1E0000," >> ./build/partitions.csv
    echo "spiffs,   data, spiffs,  0x3D0000,0x30000," >> ./build/partitions.csv

    docker run --rm -v $PWD:/project:delegated -w /project/build arduinosdk cmake "-DArduino(esp32.esp32)/Partition Scheme:STRING=Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)" -D ARDUINO_BOARD_OPTIONS_FILE=/project/BoardOptions.cmake -D CMAKE_TOOLCHAIN_FILE=../Arduino-CMake-Toolchain/Arduino-toolchain.cmake ..
fi

docker run --rm -v $PWD:/project:delegated -w /project/build arduinosdk make


# exceptionanalyse
#docker run --rm -v $PWD:/project:delegated -w /project/build arduinosdk java -jar ../EspStackTraceDecoder/EspStackTraceDecoder.jar /root/.arduino15/packages/esp32/tools/xtensa-esp32-elf-gcc/esp-2021r2-patch5-8.4.0/bin/xtensa-esp32-elf-addr2line nuki_hub.elf excdump.txt
