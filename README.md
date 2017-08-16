virtualhomeduino
================

quickly hacked together proof of concept stuff for virtualhomeduino

Added pigpio library support, configuration uses GPIO pin numbers instead of wiringPI pin numbers.
Example configuration:

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
    },

