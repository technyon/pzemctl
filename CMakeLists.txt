cmake_minimum_required(VERSION 3.0.0)

project(pzemctl CXX)

include_directories(Lib/pzem004t)
# link_directories(Lib/pzem004t)

add_definitions(-DPZEM004_NO_SWSERIAL)

add_executable(pzemctl
        pzemctl.cpp
        Lib/pzem004t/PZEM004Tv30.cpp
        pzem004t.cpp network.cpp network.h)

# Arduino.h is included in hello_world.cpp, so link with Arduino core
target_link_arduino_libraries(pzemctl
    PRIVATE
        core
        SPI
        Ethernet2
        PubSubClient)

# This is needed for the generation of HEX binary and uploading it
target_enable_arduino_upload(pzemctl)

