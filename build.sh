#!/bin/bash

echo "Install pigpio library"
sudo apt-get install pigpio

echo "Init RFControl submodule"
git submodule init
git submodule update

echo "Make vhduino"
cd virtualhomeduino/Release
make
