# pzem energy monitor firmware

# About

This is the source code for a three phase energy monitor based upon three pzem-004t modules. It reads energy values from those modules and sends them over LAN to an mqtt server. Optionally, values can be displayed on an SSD1306 display.
The included 3D printable space houses all required hardware.

# Hardware
### Required
- Arduino Mega 2560
- W5500 LAN Module (https://www.az-delivery.de/en/products/w5500-ethernet-netzwerk-internet-modul-fuer-arduino)
- SSD1306 Display (https://www.amazon.de/gp/product/B0777HHQDT/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1)

### Optional
- SSD1306 Display (https://www.amazon.de/gp/product/B0777HHQDT)
- Push buttons to control display (https://www.amazon.de/gp/product/B07DJ8QMB1). Other buttons can be used of course, Fusion 360 file is included to modify enclosure.
- Relay card to control external appliance
- 3 LEDs

# Compilation

### Prerequisites
- Arduino SDK. Unpack and set ARDUINO_SDK_PATH to its location (e. g. export ARDUINO_SDK_PATH="/usr/share/arduino-1.8.13/")
- Arduino cmake toolchain: https://github.com/a9183756-gh/Arduino-CMake-Toolchain/blob/master/README.md. Unpack to a location of your choice
- cmake
- avr-gcc

### Running cmake

In the pzemctl directory, create a cmake build directory in change into it.

- cmake -D CMAKE_TOOLCHAIN_FILE=/path/to/Arduino-toolchain.cmake ..
- You will get an error that the board is not selected.
- Run "ccmake ..". As ARDUINO_BOARD, select "Arduino Mega or Mega 2560 [avr.mega]" and configure.
- cmake -D CMAKE_TOOLCHAIN_FILE=/usr/share/Arduino-CMake-Toolchain/Arduino-toolchain.cmake -D ARDUINO_BOARD_OPTIONS_FILE=BoardOptions.cmake ..

cmake should now be setup. Run make to compile.

# Wiring

- Connect pzem-004t modules to Mega uart ports 1-3 (Mega pins 14-19)
- connect W5500 LAN module to SPI PINS (Mega pins 50-53)
- connect W5500 CS pin to Mega Pin 53
- connect W5500 Reset pin to Mega Pin 49
- connect display to Mega I2C pins (optional; Mega pins 20 and 21)
- Connect LEDs to Mega pins 5-8 (optional; PIN 7 is for network status)
- Connect Buttons to GND and Mega pins 44-48 (optional)

# Enclosure

The enclosure is seperated into two levels. The bottom levels holds the pzem-004t modules. Use M3 ~4mm screws.
The upper layer has room for the Arduino mega and the W5500 module. Again, uses M3 screws. Optionally, 3 LEDs can be mounted here. They point inside the case, I used white PETG so they can shine through.
The lid allows to mount the display (use M2 screws and nut) and the buttons.