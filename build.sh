#!/bin/sh
#CXX=$HOME/rpi/tools/arm-bcm2708/arm-bcm2708-linux-gnueabi/bin/arm-bcm2708-linux-gnueabi-gcc
CXX=$HOME/rpi/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
cd pigpio
make clean
make -j16
sudo make install
cd ..
if [ ! -e "$CXX" ]; then
   echo "Please set the correct location in the build script.\n$CXX does not exist.\n"
   exit;
fi
rm -rf *.o
rm -rf vhduino
$CXX -Wall -DRF_CONTROL_VARDUINO=1 vhduino.cpp pigpio_rfcontrol.cpp pigpio_functions.cpp -lpthread -lstdc++ -lrt -lpigpio -g -rdynamic -o vhduino
