cmake_minimum_required(VERSION 3.0.0)

set(ARDUINO_BOARD "Arduino Mega or Mega 2560 [avr.mega]")

project(pzemctl CXX)

include_directories(Lib/pzem004t)
include_directories(Lib/FreeRTOS)
include_directories(Lib/Adafruit_BusIO)
include_directories(Lib/Adafruit_GFX)
include_directories(Lib/Adafruit_SSD1306)
include_directories(Lib/PubSubClient)

add_definitions(-DPZEM004_NO_SWSERIAL -DPZEM004T_DISABLE_SEARCH)

file(GLOB SRCFILES
    "Lib/FreeRTOS/src/*.c"
    "Lib/FreeRTOS/src/*.cpp"
    "Lib/Adafruit_BusIO/*.cpp"
    "Lib/Adafruit_GFX/*.cpp"
)

add_executable(pzemctl
        ${SRCFILES}
        Config.h
        pzemctl.cpp
        Lib/pzem004t/PZEM004Tv30.cpp
        Lib/Adafruit_SSD1306/Adafruit_SSD1306.cpp
        Lib/PubSubClient/src/PubSubClient.h
        Lib/PubSubClient/src/PubSubClient.cpp
        Pzem004t.cpp
        Network.cpp
        DisplaySSD1306.cpp
        Led.cpp
        Input.cpp
        WebServer.cpp
        EepromConfiguration.cpp
)

target_link_arduino_libraries(pzemctl
    PRIVATE
        core
        SPI
        EEPROM
        Ethernet
        Wire
)

target_enable_arduino_upload(pzemctl)