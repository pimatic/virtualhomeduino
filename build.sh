#!/bin/sh
CXX=$HOME/rpi/tools/arm-bcm2708/arm-bcm2708-linux-gnueabi/bin/arm-bcm2708-linux-gnueabi-gcc
if [ ! -e "$CXX" ]; then
   echo "Please set the correct location in the build script.\n$CXX does not exist.\n"
   exit;
fi
rm -rf *.o
rm -rf vhduino
$CXX -Wall -DRF_CONTROL_VARDUINO=1 vhduino.cpp pigpio_rfcontrol.cpp pigpio_functions.cpp -pthread -lpigpio -lstdc++ -lrt -o vhduino
