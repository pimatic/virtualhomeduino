#!/bin/sh
$CXX -Wall -DRF_CONTROL_VARDUINO=1 -c vhduino.cpp 
$CXX -Wall -DRF_CONTROL_VARDUINO=1 -c wiringx_rfcontrol.cpp
$CXX -Wall -DRF_CONTROL_VARDUINO=1 -c wiringx_functions.cpp
$CXX -Wall  vhduino.o wiringx_rfcontrol.o wiringx_functions.o ./wiringX/libwiringX.a -pthread -o vhduino
rm vhduino.o wiringx_rfcontrol.o