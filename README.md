EEPROMutility
==========

[Arduino](http://arduino.cc) sketch for EEPROM management via the Serial Monitor. Allows reading and writing of any data type and testing. EEPROM is a type of memory whose values are kept when the board is turned off or a new sketch is uploaded. For more information about using EEPROM on Arduino see: http://www.arduino.cc/en/Reference/EEPROM

#### Instructions
- Download the most recent release of EEPROMutility here: https://github.com/per1234/EEPROMutility/releases
- Extract the downloaded zip file.
- Move the folder to a convenient location.
- If you are using an Arduino IDE version prior to 1.6.2 then you will need to install the EEPROM v2.0 library from here: https://github.com/arduino/ArduinoCore-avr/tree/master/libraries/EEPROM. If you are using Arduino IDE version 1.6.2 or greater then the library is already installed.
- Open EEPROMutility/EEPROMutility.ino in the Arduino IDE.
- Upload the sketch to your board.
- Tools > Serial Monitor
- From the line ending menu near the bottom right of the Serial Monitor window, select "No line ending".
- From the baud rate menu near the bottom right of the Serial Monitor window, select "9600 baud".
- Further help is available via the Serial Monitor interface.


#### Contributing
Pull requests or issue reports are welcome! Please see the [contribution rules](.github/CONTRIBUTING.md) for instructions.
