#!/bin/bash

export SERIAL_PORT=/dev/ttyACM0
cd cmake-build-debug
ninja upload-pzemctl


