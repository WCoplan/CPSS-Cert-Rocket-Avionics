// generate file name string
inline String generateName(int fileNo) {
  return "log_" + String(fileNo) + ".csv";
}

const char* newStore() {
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
  storeStart(generateName(fileNo));
  if(storeFile){
    storeFile.println("MET,Pressure,ASL,AGL,VelocityX,accelX,accelY,accelZ,gyrX,gyrY,gyrZ,batV,pitch,yaw,roll,oriA,oriB,oriC,oriD,dt,mode");
  }
  storeEnd();
  Serial.println("log init success");

  return generateName(fileNo).c_str();
}

void storeWrite(int data, bool newLine) {
  storeWriteFinal(data, newLine, true);
}

void storeWrite(float data, bool newLine) {
  storeWriteFinal(data, newLine, false);
}

void storeWriteFinal(float data, bool newLine, bool useInt) {
  // test log existence
  if (storeFile) {
    if(useInt){
      storeFile.print(int(data));
      //Serial.print(int(data));
    }
    else {
      storeFile.print(data, 7);
      //Serial.print(data, 7);
    }
    if (newLine) {
      storeFile.print("\n");
      //Serial.print("\n");
    }
    else {
      storeFile.print(CSVseparator);
      //Serial.print(CSVseparator);
    }

  } 
  else {
    Serial.println("ERROR: opening datalog.txt");
  }
}

void storeStart(String storeName) {
  storeFile = SD.open(storeName.c_str(), FILE_WRITE);
}

void storeEnd() {
  storeFile.close();
}

void logAllValues() {

  if (f_flashPointer > f_maxFlashPointer) {
    return;
  }

  union {
    float f[21];
    byte b[84];
  } data;

  data.f[0] = f_MET;
  data.f[1] = f_pressure;
  data.f[2] = f_ASL;
  data.f[3] = f_AGL;
  data.f[4] = f_velocityX;
  data.f[5] = f_accelX;
  data.f[6] = f_accelY;
  data.f[7] = f_accelZ;
  data.f[8] = f_gyrX;
  data.f[9] = f_gyrY;
  data.f[10] = f_gyrZ;
  data.f[11] = f_batV;
  data.f[12] = f_pitch;
  data.f[13] = f_yaw;
  data.f[14] = f_roll;
  data.f[15] = f_oriA;
  data.f[16] = f_oriB;
  data.f[17] = f_oriC;
  data.f[18] = f_oriD;
  data.f[19] = dt;
  data.f[20] = f_mode;
  

  f_flash.writeByteArray(f_flashPointer, data.b, 84);

  f_flashPointer += 84;
  
}

void storeAllData() {

  union {
    float f[21];
    byte b[84];
  } data;

  for (uint32_t dataPointer = 0; dataPointer < f_flashPointer; dataPointer += 84) {
    f_flash.readByteArray(dataPointer, data.b, 84);

    // record misson elapsed time value
    storeWrite(data.f[0], 0);

    // record barometer values
    storeWrite(data.f[1], 0);
    storeWrite(data.f[2], 0);
    storeWrite(data.f[3], 0);
    storeWrite(data.f[4], 0);
    //logWrite(velSmooth.getVal(), 0);

    // record acceleration values
    storeWrite(data.f[5], 0);
    storeWrite(data.f[6], 0);
    storeWrite(data.f[7], 0);

    // record gyro values
    storeWrite(data.f[8], 0);
    storeWrite(data.f[9], 0);
    storeWrite(data.f[10], 0);

    // record voltage value
    storeWrite(data.f[11], 0);

    // record quaternion values
    storeWrite(data.f[12], 0);
    storeWrite(data.f[13], 0);
    storeWrite(data.f[14], 0);
    storeWrite(data.f[15], 0);
    storeWrite(data.f[16], 0);
    storeWrite(data.f[17], 0);
    storeWrite(data.f[18], 0);

    // record loop time value
    storeWrite(data.f[19], 0);

    // record flight mode value
    storeWrite(data.f[20], 1);
    
  }
}
