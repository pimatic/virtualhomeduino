virtualhomeduino
================

quickly hacked together proof of concept stuff for virtualhomeduino

Replaced WiringX by PiGPIO Tested on Raspberry PI v1 Model B


Pin numbers in config are real GPIO pin numbers, the following configuration has been used on the RPIv1 Model B



    {
      "plugin": "homeduino",
      "active": true,
      "driver": "gpio",
      "driverOptions": {},
      "enableReceiving": true,
      "receiverPin": 17,
      "transmitterPin": 18,
      "connectionTimeout": 120000,
      "rfrepeats": 20,
      "debug": true
    },

