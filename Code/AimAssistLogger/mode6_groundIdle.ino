bool turnOff = false;

float start_groundIdle;

void setup_groundIdle() {
  tone(p_buzzer, 500, 250);
  f_logInterval = 1;
  f_modeStartTime[6] = f_MET;
  Serial.println("Mode: 3");
  f_mode = 3;
  start_groundIdle = millis();
}

void loop_groundIdle() {

  turnOff = f_MET > f_modeStartTime[6] + 10000;

  if (millis() - start_groundIdle > 5000) {
    currentStore = newStore();
    storeStart(currentStore.c_str());
    storeAllData();
    storeEnd();
    f_flash.eraseChip();
  }

  if (turnOff) {
    while (true) {
      digitalWrite(p_LED, HIGH);
      tone(p_buzzer, 2000, 1000);
      delay(2000);
    }
  }
}
