/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                             readWriteString.ino                                                               |
  |                                                               SPIMemory library                                                                |
  |                                                                   v 3.0.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  29.05.2017                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                        This program shows the method of reading a string from the console and saving it to flash memory                       |
  |                                                                                                                                               |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/
#include "src/SPIMemory.h"

uint32_t dataAddr;

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
// Required for Serial on Zero based boards
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

#if defined (SIMBLEE)
#define BAUD_RATE 250000
#define RANDPIN 1
#else
#define BAUD_RATE 115200
#define RANDPIN A0
#endif

//SPIFlash flash(SS1, &SPI1);       //Use this constructor if using an SPI bus other than the default SPI. Only works with chips with more than one hardware SPI bus
SPIFlash flash;

bool readSerialStr(String &inputStr);

void setup() {
  Serial.begin(BAUD_RATE);
#if defined (ARDUINO_SAMD_ZERO) || (__AVR_ATmega32U4__)
  while (!Serial) ; // Wait for Serial monitor to open
#endif

  flash.begin(); 

  randomSeed(analogRead(RANDPIN));
  dataAddr = 10;
  
  union {
    float f[4];
    byte b[16];
  } data;
  
  data.f[0] = 1.123;
  data.f[1] = 2.234;
  data.f[2] = 3.345;
  data.f[3] = 5.444;

  flash.writeByteArray(dataAddr, data.b, 16);
  
  Serial.println(F("Written floats: "));
  Serial.println(data.f[0]);
  Serial.println(data.f[1]);
  Serial.println(data.f[2]);
  Serial.println(data.f[3]);
  Serial.print(F("To address: "));
  Serial.println(dataAddr);

  union {
    float f[4];
    byte b[16];
  } dataOut;
  
  flash.readByteArray(dataAddr, dataOut.b, 16);
  Serial.println(F("Read floats: "));
  Serial.println(dataOut.f[0]);
  Serial.println(dataOut.f[1]);
  Serial.println(dataOut.f[2]);
  Serial.println(dataOut.f[3]);
  Serial.print(F("From address: "));
  Serial.println(dataAddr);
  
  while (!flash.eraseSector(dataAddr));
}

void loop() {

}

//Reads a string from Serial
bool readSerialStr(String &inputStr) {
  if (!Serial)
    Serial.begin(115200);
  while (Serial.available()) {
    inputStr = Serial.readStringUntil('\n');
    Serial.println(inputStr);
    return true;
  }
  return false;
}
