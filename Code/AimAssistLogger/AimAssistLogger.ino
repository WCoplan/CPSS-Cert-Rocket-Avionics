#include <LinearRegression.h>
#include "BMI088.h"
#include "src/Smoother/Smoother.h"
#include "Quaternion.h"
#include "Orientation.h"
#include "Adafruit_BMP3XX.h"
#include "bmp3_defs.h"
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <string>
#include <Servo.h>

// u_* user defined variables
#define gravity   9.80665
const float u_seaPressure = 1013.25;
const float u_launchDetect = 20.0;    // additional acceleration in m/s^2 needed to detect launch

// f_* in flight variables
float f_descentSlope;                   // Slope during parachute descent
float f_degree = 0;                     // Current attempting flap value
float f_currentFlaps;                   // Stores current degree of flaps (0--90)
int   f_modeStartTime[7];               // Stores start time in milliseconds of each loop
float f_logInterval;                    // interval between logs at any given moment
bool  f_logOveride = false;             // Overide log interval
int   f_lastLog;                        // Stores milliseconds since boot of the last log entry (used for the variable logging rate)
int   f_mode = 0;                       // Flight mode
int   f_bootStatus;                     // Status of the boot (0 == good, -1 == bad)
int   f_bootError = 0;                  // Specific boot error
float f_gyrX, f_gyrY, f_gyrZ;           // raw gyro X, Y, Z data
float f_accelX, f_accelY, f_accelZ;     // raw accel X, Y, Z data
float f_lastAccelX, f_lastAccelY, f_lastAccelZ;
float f_jerkX, f_jerkY, f_jerkZ;        // jerk in X, Y, and Z axes
float f_batV;                           // Battery and servo power voltages
float f_AGL, f_ASL, f_GL, f_pressure;   // Alt (m) AGL, ASL, and ground level above sea level
float f_temp;                           // Temperature
float f_Hz;                             // Hz since last loop
int   f_startMET, f_MET;                // Mission Ellapsed Time
float f_pitch, f_yaw, f_roll;           // LOCAL pyr, for global pyr see f_pos object
float f_oriA, f_oriB, f_oriC, f_oriD;   // Quaternion variables a, b c d
float f_velocityX, f_prevAGL;           // Vertical velocity, previous AGL, current velocity
float f_projectedAltitude = 0;          // Projected altitude in meters
float f_apogee = 0;                     // Apogee in meter
float f_groundTime = 0;                 // Projected time of reaching the ground in milliseconds
int   f_motorDeployMET = 999999;        // motor deploy MET
bool  f_motorsDeployed = false;
int   f_motorThrottle = 0;
int desiredMotorDirection = 0;
int lastBlink = 0;
bool LEDstate = false;


// random variables
char  CSVseparator = ',';               // Just a variable to store a comma as a character
String currentLog;                      // Logfile name
File  logFile;                          // Global logFile variable
int lastMicros = 0;
int currentMicros = 0;
float dt = 0.0;

// Objects:
Orientation      f_ori;               // Quaternion vehicle orientation
EulerAngles      f_pos;               // Euler vehicle orientation (derived from quaternion)
Bmi088Gyro       gyro(Wire, 0x68);    // Gyro sensor
Bmi088Accel      accel(Wire, 0x18);   // Accel sensor
Adafruit_BMP3XX  baro;                // Baro sensor
//Smoother velSmooth(4);
Smoother groundAlt(300);
Smoother groundAccelX(100);
Smoother groundAccelY(100);
Smoother groundAccelZ(100);

//pins p_*
//will put these in numerical order
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
  pinMode(p_LIN_IN1, OUTPUT);
  pinMode(p_LIN_IN2, OUTPUT);
  pinMode(p_LIN_FBACK1, INPUT);
  pinMode(p_LIN_EN1, OUTPUT);
  pinMode(p_LED, OUTPUT);

  // Init smoother objects
  //velSmooth.init();
  groundAlt.init();
  groundAccelX.init();
  groundAccelY.init();
  groundAccelZ.init();

  delay(3000);

  Serial.begin(115200);
  Serial.println("booting");

  // put your setup code here, to run once:
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

  // Baro boot sequence:
  f_bootStatus = baro.begin(0x76);
  if (f_bootStatus < 0) {
    Serial.println("ERROR: baro.begin()");
    f_bootError = 7;
  } else {
    Serial.println("SUCCESS: baro.begin()");
    baro.setTemperatureOversampling(BMP3_NO_OVERSAMPLING);
    baro.setPressureOversampling(BMP3_OVERSAMPLING_8X);
    baro.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_15); //BMP3_IIR_FILTER_COEFF_127
    baro.setOutputDataRate(BMP3_ODR_200_HZ);
  }

  // SD Boot sequence:
  if (!SD.begin(p_CS_SD)) {
    f_bootError = 8;
    Serial.println("ERROR: Card failed, or not present");
  }

  // Print boot status:
  if (f_bootError == 0) {
    Serial.println("Boot successful! Moving to Pad Idle mode.");
  } else {
    Serial.println("Boot unsuccessful. Holding.  Error = " + String(f_bootError));
    tone(p_buzzer, 500, 5000);
    while (true);
  }
  
  // Make a new log file and move to pad idle mode
  currentLog = newLog();
  setup_padIdle();
}

void loop() {
  // TODO create vector dataBuffer and push the data string to it rather than the file
  // Once the vector is full to 10 values, write to the datalog then clear the vector
  //Serial.println(f_mode);
  updateBaro();
  updateAccel();
  updateGyro();
  updateVoltages();
  updateAttitudeFromQuat();
  
  switch (f_mode) {
    case 1: // pad idle
      loop_padIdle();
      break;
    case 2: // powered ascent
      loop_flight();
      break;
    case 3: // ground idle
      loop_groundIdle();
      break;
  }


  currentMicros = micros();
  dt = (currentMicros - lastMicros) / 1000000.0; //time diff in seconds
  lastMicros = currentMicros;

  f_ori.update(f_gyrZ, f_gyrY, f_gyrX, dt);

  if(millis() > lastBlink + 500){
    digitalWrite(p_LED, LEDstate);
    LEDstate = !LEDstate;
    lastBlink = millis();
  }

  // update mission elapsed time value
  if (f_mode == 1) {
    f_MET = 0;
  }
  else {
    f_MET = millis() - f_startMET;
  }

  if ((millis() - f_lastLog >= 1000 / f_logInterval) || f_logOveride) {
    logAllValues();
    f_lastLog = millis();
    f_logOveride = false;
  }
}
