#!/bin/bash

WD=$(pwd)
echo "Working directory: ${WD}"

git clone https://github.com/joan2937/pigpio
git clone --depth=1 https://github.com/raspberrypi/tools

echo "Init RFControl submodule"
git submodule init
git submodule update


export CROSS_PREFIX="${WD}/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf-"
export PIGPIO_SRC="${WD}/pigpio"

echo "Make pigpio"
sed -i "s/CROSS_PREFIX =//" ${PIGPIO_SRC}/Makefile
cd pigpio
make

echo "Make vhduino"
cd ../virtualhomeduino/Release
make

