//IMU
#include "library/ICM-20608-G.h"
ICM_20608_G imu(GyroRange::G_500DEGS, AccelRange::A_4_g);

//MAG
#include "Adafruit_MLX90395.h"
Adafruit_MLX90395 sensor = Adafruit_MLX90395();

//GPS
#include "Adafruit_GPS.h"
#define GPSSerial Serial2

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

uint32_t timer = millis();

//BAR
#include <SPI.h>
#include <107-Arduino-BMP388.h>

static int const BMP388_CS_PIN  = 2;
static int const BMP388_INT_PIN = 6;

using namespace drone;

ArduinoBMP388 bmp388([](){ digitalWrite(BMP388_CS_PIN, LOW); },
                     [](){ digitalWrite(BMP388_CS_PIN, HIGH); },
                     [](uint8_t const d) -> uint8_t { return SPI.transfer(d); },
                     [](unit::Pressure const pressure)
                     {
                         Serial.print(pressure.value() / 100.0);
                         Serial.print(" hPa / ");
                         Serial.print(ArduinoBMP388::convertPressureToAltitude(pressure).value());
                         Serial.println(" m");
                     },
                     [](unit::Temperature const temperature)
                     {
                         Serial.print(temperature.value() + 273.15);
                         Serial.println(" °C");
                     });
void setup(void)
{
  SPI.begin();
  delay(10);
  Serial.begin(9600);
  while (!Serial) {
      delay(10);
  }
  Serial.println("Adafruit GPS library basic parsing test!");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
  
  
  /* Print data of BMP388 sensor. */
  Serial.print(bmp388);

  /* Setup SPI access */
  SPI.begin();
  pinMode(BMP388_CS_PIN, OUTPUT);
  digitalWrite(BMP388_CS_PIN, HIGH);

  /* Attach interrupt handler */
  pinMode(BMP388_INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BMP388_INT_PIN), [](){ bmp388.onExternalEventHandler(); }, FALLING);

  /* Configure BMP388 */
  bmp388.begin(BMP388::OutputDataRate::ODR_12_5_Hz);

  imu.init();
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
