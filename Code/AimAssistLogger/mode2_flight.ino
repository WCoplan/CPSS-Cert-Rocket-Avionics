void setup_flight() {
  tone(p_buzzer, 500, 250);
  f_logInterval = 200;
  f_modeStartTime[2] = f_MET;
  Serial.println("Mode: 2");
  f_mode = 2;
}

void loop_flight() {
  
  if ((f_AGL < 10 || f_MET > 300000) && (f_MET > 10000)) {
    setup_groundIdle();
  }

}
