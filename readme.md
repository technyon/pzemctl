# pzem energy monitor firmware

## Compilation

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