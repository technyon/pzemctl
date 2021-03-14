cmake_minimum_required(VERSION 3.0.0)

project(pzemctl CXX)

include_directories(Lib/pzem004t)
include_directories(Lib/FreeRTOS)
include_directories(Lib/Adafruit_BusIO)
include_directories(Lib/Adafruit_GFX)
include_directories(Lib/Adafruit_SSD1306)

add_definitions(-DPZEM004_NO_SWSERIAL)

file(GLOB SRCFILES
    "Lib/FreeRTOS/src/*.c"
    "Lib/FreeRTOS/src/*.cpp"
    "Lib/Adafruit_BusIO/*.cpp"
    "Lib/Adafruit_GFX/*.cpp"
)

add_executable(pzemctl
        ${SRCFILES}
        pzemctl.cpp
        Lib/pzem004t/PZEM004Tv30.cpp
        Lib/Adafruit_SSD1306/Adafruit_SSD1306.cpp
        Pzem004t.cpp
        Network.cpp
        DisplaySSD1306.cpp
        Led.cpp
)

# Arduino.h is included in hello_world.cpp, so link with Arduino core
target_link_arduino_libraries(pzemctl
    PRIVATE
        core
        SPI
        Ethernet
        PubSubClient
        Wire
)

# This is needed for the generation of HEX binary and uploading it
target_enable_arduino_upload(pzemctl)

