//
// TODO
// [Y] Temperature
// [Y] Humidity
// [Y] PM
// [Y] NO2
// [] SO2
// [Y] Ambient noise in dB
// [] Wind speed
// [] Wind dir
// [] Rainfall

// [] Warmup period
// [] Averaging
// [] Blues upload
// [] Timeout + soft reset

#include "Seeed_SHT35.h"
#include <Wire.h>
#include "Adafruit_PM25AQI.h"
#include <SoftwareSerial.h>
#include "DFRobot_MultiGasSensor.h"

#define SDAPIN  A4
#define SCLPIN  A5
#define RSTPIN  2

#define PCBARTISTS_DBM       0x48
#define I2C_REG_DECIBEL      0x0A
#define I2C_ADD_NO2          0x74

SHT35 sensor(SCLPIN);
SoftwareSerial pmSerial(5, 6);

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
DFRobot_GAS_I2C gas(&Wire ,I2C_ADD_NO2);

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("Serial start");

  //Check SHT35 connection
  if(sensor.init()){
    Serial.println("SHT35 sensor init failed");
  }

  //Check Plantower connection
  pmSerial.begin(9600); 
  if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
    Serial.println("PM 2.5 sensor init failed");
  }

  //Check NO2 sensor connection
  if(!gas.begin())
  {
    Serial.println("NO2 sensor init failed");
  }
  gas.changeAcquireMode(gas.PASSIVITY);
  gas.setTempCompensation(gas.ON);
}

void loop() {
  Serial.print("SHT35 Temperature = ");   Serial.print(SHT_temp());    Serial.println(" ℃ ");
  Serial.print("SHT35 Humidity = "); Serial.print(SHT_hum()); Serial.println(" % ");
  Serial.print("Sound level = "); Serial.print(sound_db()); Serial.println(" dB ");
  Serial.print("PM 1.0 = "); Serial.print(PMS_pm10()); Serial.println(" μg/m3 ");
  Serial.print("PM 2.5 = "); Serial.print(PMS_pm25()); Serial.println(" μg/m3 ");
  Serial.print("PM 10 = "); Serial.print(PMS_pm100()); Serial.println(" μg/m3 ");
  Serial.print("NO2 = "); Serial.print(DFR_NO2()); Serial.println(" PPM ");
  Serial.println("   ");
  delay(1000);
}

float SHT_temp(){
  //Get temperature from SHT35 sensor
  float temp,hum;
  if(NO_ERROR!=sensor.read_meas_data_single_shot(HIGH_REP_WITH_STRCH,&temp,&hum))
  {
    Serial.println("read temp failed!!");
  }
  return temp;
}

float SHT_hum(){
  //Get humidity from SHT35 sensor
  float temp,hum;
  if(NO_ERROR!=sensor.read_meas_data_single_shot(HIGH_REP_WITH_STRCH,&temp,&hum))
  {
    Serial.println("read hum failed!!");
  }
  return hum;
}

byte sound_db(){
  byte sound_level = reg_read(PCBARTISTS_DBM, I2C_REG_DECIBEL);
  return sound_level;
}

byte reg_read(byte addr, byte reg){
  //Read from Decibel meter register
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(addr, (uint8_t)1);
  byte data = Wire.read();
  return data;
}

int PMS_pm10(){
  //Get PM 1.0 standard concentration from Plantower sensor
  PM25_AQI_Data data;
  bool AQI_read = 0;
  while (AQI_read == 0){
    if (! aqi.read(&data)) {
      //Serial.println("Could not read from AQI");
      delay(500);
    }
    else {
      AQI_read = 1;
    }
  }
  return data.pm10_standard;
}

int PMS_pm25(){
  //Get PM 2.5 standard concentration from Plantower sensor
  PM25_AQI_Data data;
  bool AQI_read = 0;
  while (AQI_read == 0){
    if (! aqi.read(&data)) {
      //Serial.println("Could not read from AQI");
      delay(500);
    }
    else {
      AQI_read = 1;
    }
  }
  return data.pm25_standard;
}

int PMS_pm100(){
  //Get PM 2.5 standard concentration from Plantower sensor
  PM25_AQI_Data data;
  bool AQI_read = 0;
  while (AQI_read == 0){
    if (! aqi.read(&data)) {
      //Serial.println("Could not read from AQI");
      delay(500);
    }
    else {
      AQI_read = 1;
    }
  }
  return data.pm100_standard;
}

float DFR_NO2(){
  //Get NO2 concentration from DFRobot sensor
  return gas.readGasConcentrationPPM();
}
