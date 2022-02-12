int lastBeep = 0;


void setup_padIdle() {
  tone(p_buzzer, 1000, 250);
  f_logInterval = 1;
  logStart(currentLog.c_str());
  dt = 0;
  f_modeStartTime[1] = millis();
  Serial.println("Mode: 1");
  
  f_mode = 1;
}


void loop_padIdle() {
  // Constantly smooth barometer and accel readings to get baseline values:
  groundAlt.update(f_ASL);
  groundAccelX.update(f_accelX);
  groundAccelY.update(f_accelY);
  groundAccelZ.update(f_accelZ);

  // Update ground altitude from smoothed data:
  f_GL = groundAlt.getVal();

  // Update the orientation based on the smoothed accel data:
  f_ori.updateGravity(groundAccelX.getVal(), groundAccelY.getVal(), groundAccelZ.getVal());
  f_ori.applyComplementary(f_ori.worldGravity, 0.9);
  f_ori.zeroRoll();

  // If we start to move up, change modes!
  if (f_accelY > gravity + u_launchDetect && millis() > 150000) {
    f_logOveride = true;
    f_startMET = millis();
    setup_flight();
  }

  Serial.println(f_accelY);

  if (millis() > 150000) {
    if (millis() > lastBeep + 1000) {
      tone(p_buzzer, 500, 50);
      lastBeep = millis();
    }
  }
}
