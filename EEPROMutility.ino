//EEPROMutility - EEPROM management via the serial monitor. Allows reading and writing of any data type and testing. http://github.com/per1234/EEPROMutility
#define __STDC_LIMIT_MACROS  //used for the data type menu help
#include <limits.h>  //used for the data type menu help
#include <EEPROM.h>
#include <SPI.h>  //for the ethernet library
#include "Ethernet.h"  //for IPAddress data type

#define READ_MACRO value; EEPROM.get(address, value); Serial.println(value)
#define WRITE_MACRO writeValue = Serial.parseInt(); EEPROM.put(address, writeValue)

const byte timeoutDuration = 20;
const byte progressIndicatorInterval = 30;  //every n number of EEPROM operations the progress indicator will be updated

char menu;
char type;
byte typeSize;


void setup() {
  Serial.begin(9600);
  Serial.setTimeout(timeoutDuration);  //this removes the one second lag on Serial.parseInt() calls caused by the default 1000ms timeout. If serial input is being truncated try increasing this value.
  while (!Serial) {}; //for Leonardo et al. Wait for the serial monitor to be opened before proceeding with the sketch.
  Serial.print(F("EEPROM Utility"));
}


void loop() {
  Serial.print(F("\n\nTotal EEPROM size:"));
  printBytes(E2END + 1);
  Serial.print(F("[R]ead, [W]rite, [T]est, [H]elp: "));
  while (true) {
    while (!Serial.available()) {};
    menu = Serial.read();
    if (menu == 'r' || menu == 'R') {
      menu = 'R';
    }
    else if (menu == 'w' || menu == 'W') {
      menu = 'W';
    }
    else if (menu == 't' || menu == 'T') {
      menu = 'T';
    }
    else if (menu == 'h' || menu == 'H') {
      menu = 'H';
    }
    else {
      continue;
    }
    Serial.println(menu);
    break;
  }
  if (menu == 'H') {  //main menu help
    Serial.println(F("[R]ead - Displays the contents of the EEPROM over the specified range for the specified data type."));
    Serial.println(F("[W]rite - Write the specified data to the specified EEPROM address as the specified data type."));
    Serial.println(F("[T]est - Test EEPROM. This will erase all data in the EEPROM."));
  }

  else if (menu == 'R') {  //read
    dataTypeMenu();
    unsigned int addressStart;
    unsigned int addressEnd;
    while (true) {
      Serial.print(F("Start address: "));
      while (!Serial.available()) {};
      addressStart = Serial.parseInt();  //get incoming byte
      if (addressStart <= E2END) {
        Serial.println(addressStart);
        break;
      }
      error();
    }
    while (true) {
      Serial.print(F("End address: "));
      while (!Serial.available()) {};
      addressEnd = Serial.parseInt();  // get incoming byte:
      if (addressEnd <= E2END && addressEnd >= addressStart) {
        Serial.println(addressEnd);
        break;
      }
      error();
    }
    boolean terminatorReached = false;
    for (unsigned int address = addressStart; address <= addressEnd; address += typeSize) {
      Serial.print(F("Address "));
      Serial.print(address);
      Serial.print(F(":"));
      if (type == 'T') {
        const byte value = EEPROM.read(address);
        for (byte bitCounter = 0; bitCounter < 8; bitCounter++) {
          Serial.print(F("\nbit "));
          Serial.print(bitCounter);
          Serial.print(F("="));
          Serial.print(bitRead(value, bitCounter));
        }
        Serial.println();
      }
      else if (type == 'B') {
        Serial.println(EEPROM.read(address));
      }
      else if (type == 'N') {
        int8_t READ_MACRO;
      }
      else if (type == 'C') {
        char READ_MACRO;
      }
      else if (type == 'I') {
#if defined(ARDUINO_ARCH_SAM)
        int16_t READ_MACRO;
#else
        int READ_MACRO;
#endif
      }
      else if (type == 'U') {
#if defined(ARDUINO_ARCH_SAM)
        uint16_t READ_MACRO;
#else
        unsigned int READ_MACRO;
#endif
      }
      else if (type == 'L') {
        long READ_MACRO;
      }
      else if (type == 'G') {
        unsigned long READ_MACRO;
      }
      else if (type == 'F') {
        float READ_MACRO;
      }
#if defined(ARDUINO_ARCH_SAM)
      else if (type == 'D') {
        double READ_MACRO;
      }
#endif
      else if (type == 'P') {
        IPAddress READ_MACRO;
      }
      else if (type == 'S') {
        for (; address <= E2END && (terminatorReached == false || address <= addressEnd); address++) {
          char value;
          EEPROM.get(address, value);
          Serial.print(value);
          if (value == 0) {
            terminatorReached = true; //set the flag that a string terminator has been reached this forces the sketch to read at least one string even if it needs to read beyond the endAddress to do so
            break;
          }
        }
        Serial.println();
      }
    }
  }

  else if (menu == 'W') {  //write
    dataTypeMenu();
    unsigned int address;
    while (true) {
      Serial.print(F("Address: "));
      while (!Serial.available()) {};
      address = Serial.parseInt();  //get incoming byte
      Serial.println(address);
      if (address + typeSize - 1 <= E2END ) {
        break;
      }
      error();
    }
    byte addressBit;
    if (type == 'T') {
      while (true) {
        Serial.print(F("Bit: "));
        while (!Serial.available()) {};
        addressBit = Serial.parseInt();  //get incoming byte
        if (addressBit < 8) {
          Serial.println(addressBit);
          break;
        }
        Serial.println(F("Error: bit value must be 0-7"));
      }
    }
    Serial.print(F("Write value: "));
    while (!Serial.available()) {};
    if (type == 'T') {
      const byte writeValue = Serial.parseInt();  //get incoming byte
      byte value = EEPROM.read(address);
      bitWrite(value, writeValue, addressBit);
      EEPROM.update(address, value);
      value = EEPROM.read(address);
      Serial.println(bitRead(value, addressBit));
    }
    else if (type == 'B') {
      EEPROM.update(address, Serial.parseInt());
      Serial.println(EEPROM.read(address));
    }
    else if (type == 'N') {
      const int8_t WRITE_MACRO;
      int8_t READ_MACRO;
    }
    else if (type == 'C') {
      const char writeValue = Serial.read();  //get incoming byte
      EEPROM.put(address, writeValue);
      char READ_MACRO;
    }
    else if (type == 'I') {
#if defined(ARDUINO_ARCH_SAM)
      const int16_t WRITE_MACRO;
      int16_t READ_MACRO;
#else
      const int WRITE_MACRO;
      int READ_MACRO;
#endif
    }
    else if (type == 'U') {
#if defined(ARDUINO_ARCH_SAM)
      const uint16_t WRITE_MACRO;
      uint16_t READ_MACRO;
#else
      const unsigned int WRITE_MACRO;
      unsigned int READ_MACRO;
#endif
    }
    else if (type == 'L') {
      const long WRITE_MACRO;
      long READ_MACRO;
    }
    else if (type == 'G') {
      const unsigned long WRITE_MACRO;
      unsigned long READ_MACRO;
    }
    else if (type == 'F') {
      const float writeValue = Serial.parseFloat();  //get incoming byte
      EEPROM.put(address, writeValue);
      float READ_MACRO;
    }
#if defined(ARDUINO_ARCH_SAM)
    else if (type == 'D') {
      const float writeValue = Serial.parseFloat();  //get incoming byte
      EEPROM.put(address, writeValue);
      double READ_MACRO;
    }
#endif
    else if (type == 'P') {
      byte writeValue[4];
      for (byte octet = 0; octet < 4; octet++) {
        writeValue[octet] = Serial.parseInt();
      }
      const IPAddress writeIP = IPAddress(writeValue);
      EEPROM.put(address, writeIP);
      IPAddress READ_MACRO;
    }
    else if (type == 'S') {
      const unsigned int lengthMax = 256;
      char writeValue[lengthMax];
      const byte bytesWritten = Serial.readBytesUntil(0, writeValue, lengthMax - 1);
      writeValue[bytesWritten] = 0; //add terminator
      EEPROM.put(address, writeValue);
      char value[lengthMax];
      EEPROM.get(address, value);
      Serial.println(value);
    }
    Serial.println(F("Write complete"));
  }

  else if (menu == 'T') {  //test EEPROM
    Serial.print(F("WARNING! All data in EEPROM will be erased. Continue(Y/N): "));
    while (!Serial.available()) {};
    const char confirmValue = Serial.read(); //get incoming byte
    Serial.println(confirmValue);
    if (confirmValue == 'y' || confirmValue == 'Y') {
      Serial.println(F("Test in progress, this may take a minute"));
      const byte progressBarIndentLength = 17;
      const byte progressBarBracketLength = 3;
      const byte progressBarLength = (E2END + 1) / progressIndicatorInterval;
      const byte progressStringLength = 8;
      for (unsigned int counter = 1; counter <= progressBarIndentLength + progressBarLength; counter++) { //center title on the progress scale bar
        Serial.write(32);
        if (counter == progressBarIndentLength - progressBarBracketLength) {
          Serial.print (F("0%["));
          counter += progressBarBracketLength;
        }
        if (counter == progressBarIndentLength + (progressBarLength - progressStringLength) / 2) {
          Serial.print(F("Progress"));
          counter += progressStringLength;
        }
      }
      Serial.println(F("]100%"));
      Serial.print(F("Clearing EEPROM: "));
      unsigned int previousAddress = 0;
      for (unsigned int address = 0; address <= E2END; address++) {  //set all addresses to 0
        EEPROM.update(address, 0);
        progressIndicator(address, previousAddress);
      }
      Serial.print(F("\nWriting EEPROM:  "));
      previousAddress = 0;
      for (unsigned int address = 0; address <= E2END; address++) {  //set all addresses to 255
        EEPROM.update(address, 255);
        progressIndicator(address, previousAddress);
      }
      Serial.print(F("\nChecking EEPROM: "));
      previousAddress = 0;
      boolean success = true;
      for (unsigned int address = 0; address <= E2END; address++) {  //verify that all addresses == 255
        progressIndicator(address, previousAddress);
        if (EEPROM.read(address) != 255) {
          Serial.print(F("\nTest failed, address: "));
          Serial.println(address);
          success = false;
        }
      }
      if (success == true) {  //no errors encountered
        Serial.println(F("\nEEPROM Test passed"));
      }
    }
  }
}


void dataTypeMenu() {
#if defined(ARDUINO_ARCH_SAM)
  Serial.print(F("bi[T], [B]yte, i[N]t8_t, [C]har, [I]nt16_t, [U]int16_t, [L]ong, unsi[G]ned long, [F]loat, [D]ouble, I[P]Address, [S]tring, [H]elp: "));
#else
  Serial.print(F("bi[T], [B]yte, i[N]t8_t, [C]har, [I]nt, [U]nsigned int, [L]ong, unsi[G]ned long, [F]loat, I[P]Address, [S]tring, [H]elp: "));
#endif
  while (true) {
    while (!Serial.available()) {};
    type = Serial.read();
    if (type == 't' || type == 'T') {
      type = 'T';
      typeSize = 1;
    }
    else if (type == 'b' || type == 'B') {
      type = 'B';
      typeSize = sizeof(byte);
    }
    else if (type == 'n' || type == 'N') {
      type = 'N';
      typeSize = sizeof(int8_t);
    }
    else if (type == 'c' || type == 'C') {
      type = 'C';
      typeSize = sizeof(char);
    }
    else if (type == 'i' || type == 'I') {
      type = 'I';
#if defined(ARDUINO_ARCH_SAM)
      typeSize = sizeof(int16_t);
#else
      typeSize = sizeof(int);
#endif
    }
    else if (type == 'u' || type == 'U') {
      type = 'U';
#if defined(ARDUINO_ARCH_SAM)
      typeSize = sizeof(uint16_t);
#else
      typeSize = sizeof(unsigned int);
#endif
    }
    else if (type == 'l' || type == 'L') {
      type = 'L';
      typeSize = sizeof(long);
    }
    else if (type == 'g' || type == 'G') {
      type = 'G';
      typeSize = sizeof(unsigned long);
    }
    else if (type == 'f' || type == 'F') {
      type = 'F';
      typeSize = sizeof(float);
    }
#if defined(ARDUINO_ARCH_SAM)
    else if (type == 'd' || type == 'D') {
      type = 'D';
      typeSize = sizeof(double);
    }
#endif
    else if (type == 'p' || type == 'P') {
      type = 'P';
      typeSize = sizeof(IPAddress);
    }
    else if (type == 's' || type == 'S') {
      type = 'S';
      typeSize = sizeof(char);
    }
    else if (type == 'h' || type == 'H') {  //display data type help
      Serial.println(type);
      Serial.println(F("Each data type can store a different range of values. To conserve memory you should use the smallest data type suitable for your application. For more information on data types see the Arduino reference: arduino.cc/en/Reference"));
      Serial.print(F("bi[T]: 0 to 1, 1/"));
      Serial.print(CHAR_BIT);
      Serial.println(F(" byte"));
      Serial.print(F("[B]yte: 0 to "));
      Serial.print(UINT8_MAX);
      printCommaBytes(sizeof(byte));
      Serial.print(F("i[N]t8_t: "));
      Serial.print(INT8_MIN);
      Serial.print(F(" to "));
      Serial.print(INT8_MAX);
      printCommaBytes(sizeof(int8_t));
      Serial.print(F("[C]har: any character"));
      printCommaBytes(sizeof(char));
#if defined(ARDUINO_ARCH_SAM)
      Serial.print(F("[I]nt16_t: "));
#else
      Serial.print(F("[I]nt: "));
#endif
      Serial.print(INT16_MIN);
      Serial.print(F(" to "));
      Serial.print(INT16_MAX);
#if defined(ARDUINO_ARCH_SAM)
      printCommaBytes(sizeof(int16_t));
#else
      printCommaBytes(sizeof(int));
#endif
#if defined(ARDUINO_ARCH_SAM)
      Serial.print(F("[U]int16_t: 0 to "));
#else
      Serial.print(F("[U]nsigned int: 0 to "));
#endif
      Serial.print(UINT16_MAX);
#if defined(ARDUINO_ARCH_SAM)
      printCommaBytes(sizeof(uint16_t));
#else
      printCommaBytes(sizeof(unsigned int));
#endif
      Serial.print(F("[L]ong: "));
      Serial.print(INT32_MIN);
      Serial.print(F(" to "));
      Serial.print(INT32_MAX);
      printCommaBytes(sizeof(long));
      Serial.print(F("unsi[G]ned long: 0 to "));
      Serial.print(UINT32_MAX);
      printCommaBytes(sizeof(unsigned long));
      Serial.print(F("[F]loat: -3.4028235E+38 to 3.4028235E+38"));
      printCommaBytes(sizeof(float));
#if defined(ARDUINO_ARCH_SAM)
      Serial.print(F("[D]ouble:"));
      printBytes(sizeof(double));
#endif
      Serial.print(F("I[P]Address: 0.0.0.0 to 255.255.255.255"));
      printCommaBytes(sizeof(IPAddress));
      Serial.print(F("[S]tring: char array, (number of characters) * "));
      Serial.print(sizeof(char));
      Serial.print(F(" +"));
      printBytes(1);
      Serial.print(F("Enter your selection: "));
      continue;
    }
    else {  //invalid input
      continue;
    }
    Serial.println(type);
    break;  //continue
  }
}


void progressIndicator(const unsigned int address, unsigned int &previousAddress) {
  if (address - previousAddress >= progressIndicatorInterval) {
    previousAddress = address;
    Serial.write(-99);
  }
}


void error() {
  Serial.println(F("Error: invalid address"));
}


void printCommaBytes(const unsigned int byteInput) {
  Serial.print(F(","));
  printBytes(byteInput);
}


void printBytes(const unsigned int byteInput) {
  Serial.print(' ');
  Serial.print(byteInput);
  Serial.print(F(" byte"));
  if (byteInput > 1) {
    Serial.println('s');
    return;
  }
  Serial.println();
}

