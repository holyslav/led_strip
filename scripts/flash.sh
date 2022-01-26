#!/bin/bash
source ../rtos/export.sh
export ESPPORT=/dev/ttyUSB0
cmake -B build && cmake --build build --parallel --target flash