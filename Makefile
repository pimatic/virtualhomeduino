
#
# Makefile cross compiling vhduino for raspberry pi on a linux box
#

WORKING_DIRECTORY = $(shell pwd)
CROSS_PREFIX = $(WORKING_DIRECTORY)/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf-
PIGPIO_SRC = pigpio
CPP = $(CROSS_PREFIX)g++
CFLAGS = -Wall -O3
LDFLAGS = -lpthread -lpigpio -lrt

# commits/versions/revisions to checkout for RFControl and pigpio
RFCONTROL_COMMIT = 70413e8
PIGPIO_COMMIT = 1aa4cca # pigpio V64

OBJS = pigpio_functions.o pigpio_rfcontrol.o virtualhomeduino.o
BINARY = vhduino

all: $(BINARY)

$(BINARY): tools RFControl pigpio $(OBJS)
	$(CPP) $(CFLAGS) -L${PIGPIO_SRC} -o $(BINARY) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CPP) $(CFLAGS) -DRF_CONTROL_VARDUINO=1 -I $(PIGPIO_SRC) -c "$<"

pigpio:
	git clone https://github.com/joan2937/pigpio
	cd pigpio; git checkout $(PIGPIO_COMMIT)
	sed -i "s/CROSS_PREFIX =//" ${PIGPIO_SRC}/Makefile
	export CROSS_PREFIX=$(CROSS_PREFIX); make -C ${PIGPIO_SRC}

tools:
	git clone --depth=1 https://github.com/raspberrypi/tools

RFControl:
	git clone https://github.com/pimatic/RFControl.git
	cd RFControl; git checkout $(RFCONTROL_COMMIT)

clean:
	rm -rf $(OBJS) $(BINARY) pigpio RFControl

.PHONY: all clean
