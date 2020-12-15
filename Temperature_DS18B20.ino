/**********************************************************
**Name:     CPU WakeUp
**Function: CPU wake up routine excute DS18B20 temperature application
**Input:    none
**Output:   none
**note:     this function is define multiple times for different applicatons
**********************************************************/

#ifdef TEMPERATURE_APPLICATION

char sensorRead;
char samples = 5;

void CPU_WakeUp(void) {

  switch (sensorRead) {

    case 1:
      temp_1_Reading = 0;
      for (int tempSample = 0; tempSample < samples; tempSample++) {
        sensor_One.requestTemperatures();
        temp_1_Reading += sensor_One.getTempC(sensor1);
        delay(50);
      }
      temp_1_Reading = temp_1_Reading / samples;
      printAddress(sensor1);
      Serial.print(" Temp C: ");
      Serial.println(temp_1_Reading);

      if (temp_1_Reading > 35 || temp_1_Reading < 0) {
        sensorRead = 1;
        Serial.println("Temp Error retrying");
      } else {
        sensorRead = 2;
      }
      break;

    case 2:
      temp_2_Reading = 0;
      for (int tempSample = 0; tempSample < samples; tempSample++) {
        sensor_Two.requestTemperatures();
        temp_2_Reading += sensor_Two.getTempC(sensor2);
        delay(50);
      }
      temp_2_Reading = temp_2_Reading / samples;
      printAddress(sensor2);
      Serial.print(" Temp C: ");
      Serial.println(temp_2_Reading);

      if (temp_2_Reading > 35 || temp_2_Reading < 0) {
        sensorRead = 2;
        Serial.println("Temp Error retrying");
      } else {
        sensorRead = 3;
      }
      break;

    case 3:
      temp_3_Reading = 0;
      for (int tempSample = 0; tempSample < samples; tempSample++) {
        sensor_Three.requestTemperatures();
        temp_3_Reading += sensor_Three.getTempC(sensor3);
        delay(50);
      }
      temp_3_Reading = temp_3_Reading / samples;
      printAddress(sensor3);
      Serial.print(" Temp C: ");
      Serial.println(temp_3_Reading);

      if (temp_3_Reading > 35 || temp_3_Reading < 0) {   //if temperature error, restart measurement
        sensorRead = 3;
        Serial.println("Temp Error retrying");
      } else {
        sensorRead = 4;
      }
      break;

    case 4:
      temp_4_Reading = 0;
      for (int tempSample = 0; tempSample < samples; tempSample++) {
        sensor_Four.requestTemperatures();
        temp_4_Reading += sensor_Four.getTempC(sensor4);
        delay(50);
      }
      temp_4_Reading = temp_4_Reading / samples;
      printAddress(sensor4);
      Serial.print(" Temp C: ");
      Serial.println(temp_4_Reading);

      if (temp_4_Reading > 35 || temp_4_Reading < 0) {
        sensorRead = 4;
        Serial.println("Temp Error retrying");
      } else {
        sensorRead = 1;
      }
      APP_STATE()  = CPU_DATA_STORE;
      break;

    default:
      /*check and make sure all sensors are present and working*/
            if (!sensor_One.getAddress(sensor1, 0)) Serial.println("Unable to find address for Sensor 1");
            else if (!sensor_Two.getAddress(sensor2, 0)) Serial.println("Unable to find address for Sensor 2");
            else if (!sensor_Three.getAddress(sensor3, 0)) Serial.println("Unable to find address for Sensor 3");
            else if (!sensor_Four.getAddress(sensor4, 0)) Serial.println("Unable to find address for Sensor 4");

      sensorRead = 1;         //read sensor in sequence starting from first port
      break;
  }
}


void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void DS18x_Temp_dataStore(int dataIndex) {
  dataStrore[dataIndex].DS18x_Temp_sen1 = temp_1_Reading;
  dataStrore[dataIndex].DS18x_Temp_sen2 = temp_2_Reading;
  dataStrore[dataIndex].DS18x_Temp_sen3 = temp_3_Reading;
  dataStrore[dataIndex].DS18x_Temp_sen4 = temp_4_Reading;
}


#endif
