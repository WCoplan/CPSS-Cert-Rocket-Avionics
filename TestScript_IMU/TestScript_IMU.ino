#include "src/ICM-20608-G.h"
#include "src/ICM-20608-G_registers.h"

ICM_20608_G imu(GyroRange::G_500_DEGS, AccelRange::A_4_G);

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
