// generate file name string
inline String generateName(int fileNo) {
  return "log_" + String(fileNo) + ".csv";
}

const char* newLog() {
  // iterate over potential log names until a unique one is generated
  int fileNo = 0;
  
  while (true) {
    //String filename = generateName(fileNo) + String(fileNo);
    String temp = generateName(fileNo);
    char filename[temp.length() + 1];
    temp.toCharArray(filename, sizeof(filename));

    if (!SD.exists(filename)) {
      Serial.println("New Log Number Found");
      Serial.println(generateName(fileNo).c_str());
      break;
    }
    else {
      fileNo++;
    }
  }

  // create log
  // String initString = "! log " + String(fileNo) + " begin !\n";
  //File dataFile = SD.open(generateName(fileNo).c_str(), FILE_WRITE);

  // verify log integrity
  logStart(generateName(fileNo));
  if(logFile){
    logFile.println("MET,Pressure,ASL,AGL,VelocityX,accelX,accelY,accelZ,gyrX,gyrY,gyrZ,batV,pitch,yaw,roll,oriA,oriB,oriC,oriD,dt,A-Project,groundTime,dSlope,apogee,currentFlap,flapSet,mode,desiredMotorDirection,motorThrottle");
  }
  logEnd();
  Serial.println("log init success");

  return generateName(fileNo).c_str();
}

void logWrite(int data, bool newLine) {
  logWriteFinal(data, newLine, true);
}

void logWrite(float data, bool newLine) {
  logWriteFinal(data, newLine, false);
}

void logWriteFinal(float data, bool newLine, bool useInt) {
  // test log existence
  if (logFile) {
    if(useInt){
      logFile.print(int(data));
      //Serial.print(int(data));
    }
    else {
      logFile.print(data, 7);
      //Serial.print(data, 7);
    }
    if (newLine) {
      logFile.print("\n");
      //Serial.print("\n");
    }
    else {
      logFile.print(CSVseparator);
      //Serial.print(CSVseparator);
    }

  } 
  else {
    Serial.println("ERROR: opening datalog.txt");
  }
}

void logStart(String logName) {
  logFile = SD.open(logName.c_str(), FILE_WRITE);
}

void logEnd() {
  logFile.close();
}

void logAllValues() {
  // record misson elapsed time value
  logWrite(f_MET, 0);

  // record barometer values
  logWrite(f_pressure, 0);
  logWrite(f_ASL, 0);
  logWrite(f_AGL, 0);
  logWrite(f_velocityX, 0);
  //logWrite(velSmooth.getVal(), 0);

  // record acceleration values
  logWrite(f_accelX, 0);
  logWrite(f_accelY, 0);
  logWrite(f_accelZ, 0);

  // record gyro values
  logWrite(f_gyrX, 0);
  logWrite(f_gyrY, 0);
  logWrite(f_gyrZ, 0);

  // record voltage value
  logWrite(f_batV, 0);

  // record quaternion values
  logWrite(f_pitch, 0);
  logWrite(f_yaw, 0);
  logWrite(f_roll, 0);
  logWrite(f_oriA, 0);
  logWrite(f_oriB, 0);
  logWrite(f_oriC, 0);
  logWrite(f_oriD, 0);

  // record loop time value
  logWrite(dt, 0);

  // record calculated values
  logWrite(f_projectedAltitude, 0);
  logWrite(f_groundTime, 0);
  logWrite(f_descentSlope, 0);
  logWrite(f_apogee, 0);

  // record flap info
  logWrite(f_currentFlaps, 0);
  logWrite(f_degree, 0);

  // record flight mode value
  logWrite(f_mode, 0);

  logWrite(desiredMotorDirection, 0);

  logWrite(f_motorThrottle, 1);
}
