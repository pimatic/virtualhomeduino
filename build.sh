#!/bin/bash

echo "Install pigpio library"
sudo apt-get install pigpio

echo "Init RFControl submodule"
git submodule init
git submodule update

export CROSS_PREFIX=""
export PIGPIO_SRC="unset"

echo "Make vhduino"
cd virtualhomeduino/Release
make
