#!/bin/bash
source ../rtos/export.sh
cmake -B build && cmake --build build --parallel --target app