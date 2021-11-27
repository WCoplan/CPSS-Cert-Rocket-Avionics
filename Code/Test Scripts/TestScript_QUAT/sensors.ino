void updateAccel() {
  accel.readSensor();
  f_accelX = accel.getAccelX_mss();
  f_accelY = -accel.getAccelY_mss();
  f_accelZ = accel.getAccelZ_mss();

  smoothAccelX.update(f_accelX);
  smoothAccelY.update(f_accelY);
  smoothAccelZ.update(f_accelZ);

  f_smoothAccelX = smoothAccelX.getVal();
  f_smoothAccelY = smoothAccelY.getVal();
  f_smoothAccelZ = smoothAccelZ.getVal();
}

void updateGyro() {
  gyro.readSensor();
  f_gyrX = gyro.getGyroX_rads();
  f_gyrY = -gyro.getGyroY_rads();
  f_gyrZ = gyro.getGyroZ_rads();
}
