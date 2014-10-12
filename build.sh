#!/bin/sh
g++ -Wall -DRF_CONTROL_VARDUINO=1 vhduino.cpp ./wiringX/libwiringX.a ./rfcontrol/RFControl.cpp -pthread -o vhduino
