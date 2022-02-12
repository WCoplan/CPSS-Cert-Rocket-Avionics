LinearRegression velReg = LinearRegression();

const int velRecentPoints = 10;
float velRecentAltitude[velRecentPoints];
int velRecentMET[velRecentPoints];
double velEQvalues[3];
int velLoopIndex = 0;

int baroReadingTime = 0;

void updateBaro() {
  baroReadingTime = millis();
  f_pressure = baro.pressure;
  f_ASL = (1.0 - pow(f_pressure / (u_seaPressure * 100.0), 0.190284)) * 287.15 / 0.0065;
  f_AGL = f_ASL - f_GL;

  velRecentAltitude[velLoopIndex] = f_AGL;
  velRecentMET[velLoopIndex] = baroReadingTime;
  velLoopIndex++;
  if (velLoopIndex > (velRecentPoints - 1)) {
    velLoopIndex = 0;
  }

  velReg.reset();
  for (int i = 0; i < velRecentPoints; i++) {
    velReg.learn(velRecentMET[i], velRecentAltitude[i]);
  }

  velReg.getValues(velEQvalues);
  f_velocityX = velEQvalues[0] * 1000.0;


  /*
    float tempVel = (f_AGL - f_prevAGL) / dt;
    if (tempVel != 0.0) {
    f_velocityX = tempVel;
    }
    if ((tempVel != 0.0) && (millis() > 10000)) {
    velSmooth.update(f_velocityX);
    //Serial.println(velSmooth.getVal());
    //Serial.println(f_velocityX);
    }
    f_prevAGL = f_AGL;*/

}


int lastJerkReading = 0;
int currentJerkReading = 0;
float jerkDT = 0.0;

void updateAccel() {
  accel.readSensor();
  f_accelX = accel.getAccelX_mss();
  f_accelY = -accel.getAccelY_mss();
  f_accelZ = accel.getAccelZ_mss();

  currentJerkReading = millis();
  jerkDT = ((currentJerkReading - lastJerkReading) / 1000.0);

  f_jerkX = (f_accelX - f_lastAccelX) / jerkDT;
  f_jerkY = (f_accelY - f_lastAccelY) / jerkDT;
  f_jerkZ = (f_accelZ - f_lastAccelZ) / jerkDT;

  f_lastAccelX = f_accelX;
  f_lastAccelY = f_accelY;
  f_lastAccelZ = f_accelZ;
  lastJerkReading = currentJerkReading;
}

void updateGyro() {
  gyro.readSensor();
  f_gyrX = gyro.getGyroX_rads();
  f_gyrY = -gyro.getGyroY_rads();
  f_gyrZ = gyro.getGyroZ_rads();
}

void updateVoltages() {
  f_batV = (((analogRead(p_VDBat) / 1023.0) * 3.3) * (51.0 + 360.0)) / 51.0;
}

void updateAttitudeFromQuat() {
  f_pos   = f_ori.toEuler();
  f_pitch = f_pos.pitch;
  f_yaw   = f_pos.yaw;
  f_roll  = f_pos.roll;
  f_oriA = f_ori.orientation.a;
  f_oriB = f_ori.orientation.b;
  f_oriC = f_ori.orientation.c;
  f_oriD = f_ori.orientation.d;
}
