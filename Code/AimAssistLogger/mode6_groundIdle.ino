bool turnOff = false;

void setup_groundIdle() {
  tone(p_buzzer, 500, 250);
  f_logInterval = 1;
  f_modeStartTime[6] = f_MET;
  Serial.println("Mode: 3");
  f_mode = 3;
}

void loop_groundIdle() {

  turnOff = f_MET > f_modeStartTime[6] + 10000;

  if (turnOff) {
    logEnd();
    while (true) {
      digitalWrite(p_LED, HIGH);
      tone(p_buzzer, 2000, 1000);
      delay(2000);
    }
  }
}
