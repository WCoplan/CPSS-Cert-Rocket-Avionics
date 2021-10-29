#include "library/ICM-20608-G.h"

ICM_20608_G imu(GyroRange::G_500DEGS, AccelRange::A_4_g);

void setup() {
  SPI.begin();
  delay(10);
  imu.init();
}

void loop() {
  float gx, gy, gz, ax, ay, az;

  imu.readGyro(gx, gy, gz);
  imu.readAccel(ax, ay, az);

  delay(1);
}
