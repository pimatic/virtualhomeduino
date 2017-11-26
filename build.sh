#!/bin/bash

#export CROSS_BASE="$HOME/rpi/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin"
#export PATH=$PATH:$CROSS_BASE
#CROSS_TOOLCHAIN="$CROSS_BASE/arm-linux-gnueabihf-"

echo "Install pigpio library"
sudo apt-get install pigpio

echo "Init RFControl submodule"
git submodule init
git submodule update

#sed -i -e "s|CROSS_PREFIX =.*$|CROSS_PREFIX = $CROSS_TOOLCHAIN|" pigpio/Makefile
#cd pigpio
#make
#cd ../virtualhomeduino/Release

echo "Make vhduino"
cd virtualhomeduino/Release
make
