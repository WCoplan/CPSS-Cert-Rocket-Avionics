#include "BMI088.h"
#include "src/Smoother/Smoother.h"
#include <Wire.h>

// u_* user defined variables
const int u_gravity          = 9.80665; // local gravity (m/s^2)
const int u_accelSmoothCount = 100;     // how many recent accel measurements to store for smoothed ground acceleration

// f_* in flight variables
int   f_bootStatus = 0;                 // Status of the boot (0 == good, -1 == bad)
int   f_bootError  = 0;                 // Specific boot error
float f_gyrX, f_gyrY, f_gyrZ;           // raw gyro X, Y, Z data
float f_accelX, f_accelY, f_accelZ;     // raw accel X, Y, Z data
float f_smoothAccelX, f_smoothAccelY, f_smoothAccelZ; // smoothed accel X, Y, Z data
float f_pitch, f_yaw, f_roll;           // pitch, yaw, roll
float f_oriA, f_oriB, f_oriC, f_oriD;   // quaternion variables a, b c d
int   f_lastBlink       = 0;            // last heartbeat LED change (ms)
bool  f_LEDstate       = false;         // heartbeat LED state
int   f_lastMicros     = 0;             // previous loop time (microseconds)
int   f_currentMicros  = 0;             // current loop time (microseconds)
float f_dt             = 0.0;           // dt from last loop to current loop (seconds)

// Objects:
Bmi088Gyro       gyro(Wire, 0x68);    // Gyro sensor
Bmi088Accel      accel(Wire, 0x18);   // Accel sensor
Smoother smoothAccelX(u_accelSmoothCount);
Smoother smoothAccelY(u_accelSmoothCount);
Smoother smoothAccelZ(u_accelSmoothCount);

//pins p_*
const int p_VDBat = A13;
const int p_GP4 = 32;
const int p_CS_Flash = 31;
const int p_CS_SD = 26;
const int p_GP3 = 25;
const int p_LIN_IN2 = 24;
const int p_GP1 = 23;
const int p_GP2 = 22;
const int p_buzzer = 21;
const int p_LIN_EN2 = 20;
const int p_LIN_FBACK2 = A3; // 17
const int p_LIN_FBACK1 = A2; // 16
const int p_LIN_IN3 = 14;
const int p_SCK = 13;
const int p_MISO = 12;
const int p_MOSI = 11;
const int p_LIN_IN4 = 10;
const int p_LIN_EN1 = 9;
const int p_TX = 8;
const int p_RX = 7;
const int p_LED = 5;
const int p_SERV2 = 4;
const int p_SERV1 = 3;
const int p_LIN_IN1 = 0;

void setup() {

  tone(p_buzzer, 1000, 250);

  // Init hardware access
  pinMode(p_LED, OUTPUT);

  // Init smoother objects
  smoothAccelX.init();
  smoothAccelY.init();
  smoothAccelZ.init();

  delay(3000);

  Serial.begin(115200);
  Serial.println("booting");

  //Accel boot sequence
  f_bootStatus = accel.begin();
  if (f_bootStatus < 0) {
    Serial.println("ERROR: accel.begin()");
    f_bootError = 1;
  } else {
    Serial.println("SUCCESS: accel.begin()");
  }

  f_bootStatus = accel.setOdr(Bmi088Accel::ODR_1600HZ_BW_280HZ);
  if (f_bootStatus < 0) {
    Serial.println("ERROR: accel.setOdr()");
    f_bootError = 2;
  } else {
    Serial.println("SUCCESS: accel.setOdr()");
  }

  f_bootStatus = accel.setRange(Bmi088Accel::RANGE_12G);
  if (f_bootStatus < 0) {
    Serial.println("ERROR: accel.setRange()");
    f_bootError = 3;
  } else {
    Serial.println("SUCCESS: accel.setRange()");
  }

  //Gyro boot sequence:
  f_bootStatus = gyro.begin();
  if (f_bootStatus < 0) {
    Serial.println("ERROR: gyro.begin()");
    f_bootError = 4;
  } else {
    Serial.println("SUCCESS: gyro.begin()");
  }

  f_bootStatus = gyro.setOdr(Bmi088Gyro::ODR_400HZ_BW_47HZ);
  if (f_bootStatus < 0) {
    Serial.println("ERROR: gyro.setOdr()");
    f_bootError = 5;
  } else {
    Serial.println("SUCCESS: gyro.setOdr()");
  }

  f_bootStatus = gyro.setRange(Bmi088Gyro::RANGE_1000DPS);
  if (f_bootStatus < 0) {
    Serial.println("ERROR: gyro.setRange()");
    f_bootError = 6;
  } else {
    Serial.println("SUCCESS: gyro.setRange()");
  }

  // Print boot status:
  if (f_bootError == 0) {
    Serial.println("Boot successful! Moving to Pad Idle mode.");
  } else {
    Serial.println("Boot unsuccessful. Holding.  Error = " + String(f_bootError));
    tone(p_buzzer, 500, 5000);
    while (true);
  }
}

void loop() {
  updateAccel();
  updateGyro();
  updateHeartbeat();

  /*
  NOTES FOR VEER (my son luv u kiddo)
  For now ignore the accel stuff, just start up the code while the
  computer is in a near-vertical position and assume that is directly up
  */
  
  // what you have access to:
  // f_accelX, f_accelY, f_accelZ     body frame linear acceleration (meter/sec^2)
  // f_gyrX, f_gyrY, f_gyrZ           body frame angular velocity (rad/sec)
  // f_dt                             dt for current sensor reading (seconds)

  // what you need to write to:
  // f_pitch, f_yaw, f_roll           euler pitch, yaw, roll angles
  // f_oriA, f_oriB, f_oriC, f_oriD   quaternion a, b, c, d

}

void updateDt() {
  f_currentMicros = micros();
  f_dt = (f_currentMicros - f_lastMicros) / 1000000.0; //time diff in seconds
  f_lastMicros = f_currentMicros;
}

// blinks heartbeat LED every second
void updateHeartbeat() {
  if(millis() > f_lastBlink + 500){
    digitalWrite(p_LED, f_LEDstate);
    f_LEDstate = !f_LEDstate;
    f_lastBlink = millis();
  }
}
