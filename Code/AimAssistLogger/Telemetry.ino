#define telemetry Serial3
void sendAllTelemetry() {
//  ['battery', 'lat', 'lon', 'height', 'time', 'alt', 'vx',
//                    'vy', 'vz', 'ax', 'ay', 'az', 'mx', 'my', 'mz' ]
  
  float data[] = {f_batV, 0, 0, f_ASL, f_MET, f_AGL, 
              f_velocityX, 0, 0, 
              f_accelX, f_accelY, f_accelZ,
              0,0,0};
  const int len = 15;
  
  Serial.println("Sending telemetry");
  for(int x = 0; x < len; x++){
    telemetry.print(data[x]);
    telemetry.print(",");
  }
  telemetry.print("\n");
    
//  data.f[0] = f_MET;
//  data.f[1] = f_pressure;
//  data.f[2] = f_ASL;
//  data.f[3] = f_AGL;
//  data.f[4] = f_velocityX;
//  data.f[5] = f_accelX;
//  data.f[6] = f_accelY;
//  data.f[7] = f_accelZ;
//  data.f[8] = f_gyrX;
//  data.f[9] = f_gyrY;
//  data.f[10] = f_gyrZ;
//  data.f[11] = f_batV;
//  data.f[12] = f_pitch;
//  data.f[13] = f_yaw;
//  data.f[14] = f_roll;
//  data.f[15] = f_oriA;
//  data.f[16] = f_oriB;
//  data.f[17] = f_oriC;
//  data.f[18] = f_oriD;
//  data.f[19] = dt;
//  data.f[20] = f_mode;
}
