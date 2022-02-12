#include "Smoother.h"

Smoother reading(100);

bool change = false;

float val;

void setup() {
  // put your setup code here, to run once:
  reading.init();
  delay(3000);
}

void loop() {
  if(change){
    val = 0.0;
  } else {
    val = 10.0;
  }
  change = !change;
  // put your main code here, to run repeatedly:
  reading.update(val);
  Serial.print(reading.getVal(),5);
  Serial.print(", ");
  Serial.println(reading.isReady());
}
