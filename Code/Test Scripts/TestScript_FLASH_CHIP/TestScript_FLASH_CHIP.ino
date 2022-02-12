#include <BufferedPrint.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <RingBuf.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <sdios.h>

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

SPIFlash flash;

bool readSerialStr(String &inputStr);
void writeDataFrame();
void readDataFrame(float out[]);

uint32_t f_initialDataPointer;
uint32_t f_dataPointer;
float f_ax;
float f_ay;
float f_az;

SdFat sd;

void setup() {
  Serial.begin(115200);
  
  flash.begin();

  f_initialDataPointer = 10;
  f_dataPointer = f_initialDataPointer;

  f_ax = 0.1f;
  f_ay = 0.2f;
  f_az = 0.3f;

  writeDataFrame();

  f_ax = 0.2f;
  f_ay = 0.4f;
  f_az = 0.6f;

  writeDataFrame();

  f_dataPointer = f_initialDataPointer;
  
  Serial.print(F("Written to address: "));
  Serial.println(f_dataPointer);

  float arr[3];

  readDataFrame(arr);

  Serial.print(arr[0]);
  Serial.print(", ");
  Serial.print(arr[1]);
  Serial.print(", ");
  Serial.print(arr[2]);
  Serial.println();
  
  while (!flash.eraseSector(f_dataPointer));

  // Test writing to the SD card

  fs:File aFile;

  aFile = sd.open("test.txt", FILE_WRITE);
  aFile.println("solutations");

  aFile.flush();
  aFile.close();
}

void loop() {
  
}

void writeDataFrame() {
  
  union {
    float f[3];
    byte b[12];
  } dataUnion;

  dataUnion.f[0] = f_ax;
  dataUnion.f[1] = f_ay;
  dataUnion.f[2] = f_az;

  flash.writeByteArray(f_dataPointer, dataUnion.b, 12);

  f_dataPointer += 12;
}

void readDataFrame(float out[]) {
  
  union {
    float f[3];
    byte b[12];
  } dataUnion;

  flash.readByteArray(f_dataPointer, dataUnion.b, 12);

  for (int i = 0; i < 3; i++) {
    out[i] = dataUnion.f[i];
  }

  f_dataPointer += 12;
}

//Reads a string from Serial
bool readSerialStr(String &inputStr) {
  while (Serial.available()) {
    inputStr = Serial.readStringUntil('\n');
    Serial.println(inputStr);
    return true;
  }
  return false;
}
