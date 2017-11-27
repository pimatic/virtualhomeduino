# virtualhomeduino

quickly hacked together proof of concept stuff for virtualhomeduino

## Changed Configuration

Added pigpio library support, configuration uses GPIO pin numbers instead of wiringPI pin numbers.
Example configuration:

```json
{
  "plugin": "homeduino",
  "driver": "gpio",
  "driverOptions": {},
  "enableReceiving": true,
  "receiverPin": 17,
  "transmitterPin": 18,
  "active": true,
  "connectionTimeout": 120000,
  "rfrepeats": 20,
  "debug": true
}
```

## Building

### Cross compile on a linux box

#### Prerequisites

* assuming you compile on Debian/Ubuntu
* in order to be able to cross compile vhduino on a normal computer running
  Debian/Ubuntu you need to install git

```
$ sudo apt-get install git
```

```
$ git clone https://github.com/layereight/virtualhomeduino.git
$ cd virtualhomeduino
$ make
```

Makefile compile steps:
* clone Raspberry Pi cross compile tool chain (raspberrypi/tools, ~190MB)
* clone RFControl with commit 70413e8
* clone pigpio with commit 1aa4cca (V64)
* cross compile pigpio
* cross compile virtualhomeduino sources (pigpio_functions.cpp, pigpio_rfcontrol.cpp, virtualhomeduino.cpp)
* cross compile vhduino binary

### Compile on a Raspberry Pi

#### Prerequisites

* assuming you compile on Raspbian
* in order to be able to compile vhduino on a Raspberry Pi with Raspbian you
  need to install the pigpio package (library and source files) and others

```
$ sudo apt-get install pigpio build-essential git
```

```
$ git clone https://github.com/layereight/virtualhomeduino.git
$ cd virtualhomeduino
$ make -f Makefile.rpi
```

Makefile.rpi compile steps:
* clone RFControl with commit 70413e8
* compile virtualhomeduino sources (pigpio_functions.cpp, pigpio_rfcontrol.cpp, virtualhomeduino.cpp)
* compile vhduino binary

## Runtime requirements

* vhduino is dynamically linked
* assuming you want to run vhduino on Raspbian you need to install the pigpio package

```
$ sudo apt-get install pigpio
```

## TODO

* Can cross compiling be achieved "easier"? 
  * without cloning 190MB of Raspberry Pi tools repository
  * check https://hackaday.com/2016/02/03/code-craft-cross-compiling-for-the-raspberry-pi/
