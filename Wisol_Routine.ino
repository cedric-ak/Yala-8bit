char sigfoxOutput;
String sigfoxResponse = "";
//uint8_t sigfoxMaxWait = 30;
/**********************************************************
**Name:     sigfoxRoutine
**Function: sigfox state machine
**Input:    none
**Output:   none
**note:
**********************************************************/

void wisolRoutine(void) {

  switch (sigfox.state) {

    case DeepSleep:
      if (!sfx_SleeepStatus) {
        sigfoxSleep();
        sfx_Init_Complete = true;
#ifdef DEBUG_MODE
        Serial.println("Wisol sleeping");
#endif
      } else {
        sigfoxSleep();
        sigfox.state = standBy;
      }
      break;

    case wakeUp:
      sigfoxWakeUp();
      sigfox.state = sendPacket;
      break;

    case sendPacket:
      packetTransmit();
      startTxTime = millis();
      sigfox.state = checkPacketSent;
      sigfoxResponse = "";
      //Serial.println("checkpacketsent");
      break;

    case checkPacketSent:
      checkSigfoxPacketSent();
      break;

    case delayBetweenSends:
      delayBeforeNextSend();
      break;

    case readDevice_ID:
      getID();
      if (!sfx_Init_Complete) {
        sigfox.state = readDevice_PAC;
      }
      break;

    case readDevice_PAC:
      getPac();
      if (!sfx_Init_Complete) {
        sigfox.state = DeepSleep;
      }
      break;

    case ping:
      sigfoxPing();
      if (!sfx_Init_Complete) {
        sigfox.state = readDevice_ID;
      }
      sigfoxPacketSetup();
      break;

    case Reset:      //reset wisol module
      Serial.println("sfx rst");
      pinMode(sigfoxReset_PIN, OUTPUT);
      sfx_Reset;
      delay(500);
      sfxReset_clr;
      led.blinkTime(MAGENTA, 2, 500);
      getID();
      break;

    case standBy:

      break;
  }

}

/**********************************************************
**Name:     packet Build
**Function: construct packet to be transmitted
**Input:    data byte and byte size
**Output:   none
**note:
**********************************************************/
void packetBuild(uint8_t byteSize, uint32_t dataByte) {
  for (int index = 0; index <  byteSize; index++) {
    packet[byteIndex] = ((dataByte >> (index * 8)) & 0xff);
    byteIndex++;
  }
}

/**********************************************************
**Name:     packet Transmit
**Function: Transmit packet frame to cloud
**Input:    calls packet build function and pass in frame data byte and size
**Output:   none
**note:
**********************************************************/
void packetTransmit(void) {
  uint8_t packetSize = 12;

#ifdef AIR_QUALITY                                 /*AQ payload structure*/
  packetBuild(2, 3015);                            //byte 0-1
  packetBuild(1, dataStrore[0].Humidity);          //byte 2
  packetBuild(1, dataStrore[1].Humidity);          //byte 3
  packetBuild(1, dataStrore[2].Humidity);          //byte 4
  packetBuild(1, dataStrore[0].Temp);              //byte 5
  packetBuild(1, dataStrore[1].Temp);              //byte 6
  packetBuild(1, dataStrore[2].Temp);              //byte 7
  packetBuild(1, dataStrore[0].IAQ);               //byte 8
  packetBuild(1, dataStrore[1].IAQ);               //byte 9
  packetBuild(1, dataStrore[2].IAQ);               //byte 10
  packetBuild(1, BatteryState());                  //byte 11
#endif

#ifdef TEMPERATURE_APPLICATION                     /*Temp payload Structure*/
  packetBuild(2, 2020);                            //byte 0-1
  packetBuild(1, dataStrore[1].DS18x_Temp_sen1);   //byte 2
  packetBuild(1, dataStrore[1].DS18x_Temp_sen2);   //byte 3
  packetBuild(1, dataStrore[1].DS18x_Temp_sen3);   //byte 4
  packetBuild(1, dataStrore[1].DS18x_Temp_sen4);   //byte 5
  packetBuild(1, dataStrore[2].DS18x_Temp_sen1);   //byte 6
  packetBuild(1, dataStrore[2].DS18x_Temp_sen2);   //byte 7
  packetBuild(1, dataStrore[2].DS18x_Temp_sen3);   //byte 8
  packetBuild(1, dataStrore[2].DS18x_Temp_sen4);   //byte 9
  packetBuild(1, BatteryState());                  //byte 10
#endif

#ifdef WATER_APPLICATION                            /* Water payload structure*/
  packetBuild(2, 1030);                             //byte 0-1
  packetBuild(4, cumulativeFlow);                   //byte 2-5
  packetBuild(2, dataStrore[0].flowRate);           //byte 6-7
  packetBuild(2, dataStrore[1].flowRate);           //byte 8-9
  packetBuild(2, dataStrore[2].flowRate);           //byte 10-11
#endif

#ifdef ELECTRICITY_APPLICATION                     /*Temp payload Structure*/

  if (!dailyPacket) {
    switch (packetToTX - 1) {
      case 0:
        //      Serial.println("Sending 0");
        packetBuild(2, 4030);                                                //byte 0-1
        packetBuild(4, sensorValues[9]);                                     //byte 2-5 which is Serial Number
        packetBuild(4, sensorValues[0]);                                     //byte 6-9 = Which is 20180
        packetBuild(2, sensorValues[0] - historicalSensorValues[0]);         // byte 10-11 = difference between this reading and the last one
        break;
      case 1:
        //      Serial.println("Sending 1");
        packetBuild(2, 4031);        //byte 0-1
        packetBuild(4, sensorValues[9]);               //byte 2-5
        packetBuild(4, sensorValues[2]);               //byte 6-9 = 20280
        packetBuild(1, BatteryState());                //byte 10
        break;
      case 2:
        //      Serial.println("Sending 2");
        packetBuild(2, 4032);        //byte 0-1
        packetBuild(4, sensorValues[9]);               //byte 2-5
        packetBuild(4, sensorValues[4]);               //byte 6-9 = 20181
        packetBuild(1, BatteryState());                //byte 10
        break;
      case 3:
        //      Serial.println("Sending 3");
        packetBuild(2, 4033);        //byte 0-1
        packetBuild(4, sensorValues[9]);               //byte 2-5
        packetBuild(4, sensorValues[5]);               //byte 6-9 = 20182
        packetBuild(1, BatteryState());                //byte 10
        break;
      case 4:
        //      Serial.println("Sending 4");
        packetBuild(2, 4034);                          //byte 0-1
        packetBuild(4, sensorValues[9]);               //byte 2-5
        packetBuild(4, sensorValues[6]);               //byte 6-9 = 20183
        packetBuild(1, BatteryState());                //byte 10
        break;
    }
  } else {
    dailyPacket = false;
    packetBuild(2, 4035);                               //byte 0-1
    packetBuild(4, sensorValues[9]);                    //byte 2-5 which is Serial Number
    packetBuild(3, sensorValues[10] / 60);              //byte 6-8 Which is 20091 epoch time and date transmit in minute);                                     //byte 6-9 = Which is 20180
    packetBuild(1, BatteryState());                     // byte 9 battery
  }

#endif


#ifdef ENERGY_MONITOR_APPLICATION                    /*Temp payload Structure*/
  //  Serial.println("To Tx");
  //  Serial.println(String(sensorValues[9][0]));
  //  Serial.println(String(sensorValues[0][0]));
  //  Serial.println(sensorValues[0][0] - historicalSensorValues[0][meters]);
  switch (packetToTX - 1) {
    case 0:
      //      Serial.println("Sending 0");
      packetBuild(2, 5000);                              //byte 0-1
      packetBuild(4, powerOnTime);                                     //byte 2-5 which is Serial Number
      break;
  }
#endif

#ifdef LIVE_MODE                                        //if in live mode transmit packet frame
  Sigfox.listen();
  getID();

  Sigfox.print(sfx_sendPacket);
  for (int index = 0; index < packetSize; index++) {
    if (packet[index] < 16) {
      Sigfox.print(String(0));
    }
    Sigfox.print(String(packet[index], HEX));
  }
  Sigfox.print("\r");

#endif

#ifdef DEBUG_MODE
  //if in debug mode print out packet frame on serail port
  Serial.print("Tx packet : ");
  for (int index = 0; index < packetSize; index++) {
    if (packet[index] < 16) {
      Serial.print(String(0));
    }
    Serial.print(String(packet[index], HEX));
  }
  Serial.println(" ");
  //  Serial.println("done transmitting");
#endif

  //  transmissionStatus = COMPLETE;

  byteIndex = 0;
  dataStrore[0].IAQ = 0;
  dataStrore[1].IAQ = 0;
  dataStrore[2].IAQ = 0;

  dataStrore[1].Temp = 0;
  dataStrore[2].Temp = 0;
  //transmissionComplete = true;
}
void delayBeforeNextSend() {
  if (millis() - completedSigfoxTime > sigfoxDelayTime) {
    //    Serial.print("Delay excuted ");
    Serial.println(millis() - completedSigfoxTime);
    transmissionStatus = FINALISED;
  }
}
void checkSigfoxPacketSent() {

#ifdef LIVE_MODE                      /*function causes bug in debug mode*/
  while (Sigfox.available() > 0) {
    sigfoxOutput = Sigfox.read();
    sigfoxResponse += sigfoxOutput;
  }

  if (sigfoxResponse != "") {
    //Serial.print("End of serial data ");
    //Serial.println(sigfoxResponse);
    transmissionStatus = COMPLETE;
    completedSigfoxTime = millis();
  }
  if (millis() - startTxTime > TxTimeOut) {
    transmissionStatus = COMPLETE;
    Serial.println("Timeout");
  }
#endif

#ifdef DEBUG_MODE
  transmissionStatus = COMPLETE;
#endif
}
/**********************************************************
**Name:     Sigfox Sleep
**Function: Set wisol module to deep sleep mode
**Input:    none
**Output:   none
**note:
**********************************************************/
void sigfoxSleep(void) {
  char output;
  String ack = "";
  wakeup_Pin_clr;              //clear wisol wake pin before deep sleep instruction
  Sigfox.print(sfx_deepSleep); //issue sigfox deep sleep instruction
  delay(10);
  while (Sigfox.available() > 0) {
    output = Sigfox.read();
    ack += output;
  }
  Serial.println(ack);
  sfx_SleeepStatus = true;
}

/**********************************************************
**Name:     Sigfox Wake up
**Function: awake the wisol module
**Input:    none
**Output:   none
**note:
**********************************************************/
void sigfoxWakeUp(void) {
  wakeup_Pin_set;
  delay(50);
  wakeup_Pin_clr;              //clear wisol GPIO 9 (wake up pin) after device has awaken from deep sleep
  sigfoxPing();                //ping device a few times before transmission
  sfx_SleeepStatus = false;
  //sigfox.state = standBy;      //do nothing after wake up (wait for next instruction)
#ifdef DEBUG_MODE
  Serial.println("wisol awake");
#endif
}
/**********************************************************
**Name:     get ID
**Function: Retrieve wisol module ID address
**Input:    none
**Output:   returns ID address
**note:
**********************************************************/
void getID(void) {
  //returns device ID
  char output;
  String ID = "";

  Sigfox.print(sfx_ID);
  delay(50);
  while (Sigfox.available() > 0) {
    output = Sigfox.read();
    ID += output;
  }
  Serial.print("Device ID:");
  Serial.print("\t");
  Serial.print(ID);

  if (ID.length() < 8) {
    sfxRstcounter++;
    Serial.println("No ID!");
    led.blinkTime(RED, 2, 500);
    if (sfxRstcounter < 3) {
      sigfox.state = Reset;
    } else {
      sfxRstcounter = 0;
      sigfox.state = standBy;
      return;
    }

  }
}

/**********************************************************
**Name:     get pack
**Function: Retrieves wisol Pac address
**Input:    none
**Output:   PAC address in HEX
**note:     two different can awake micro, alarm by the rtc or a falling edge pulse on INT1
**********************************************************/
void getPac(void) {
  //returns PAC device address
  char output;
  String PAC = "";
  Sigfox.print(sfx_PAC);
  delay(50);
  while (Sigfox.available() > 0) {
    output = Sigfox.read();
    PAC += output;
  }
  Serial.print("PAC:");
  Serial.print("\t");
  Serial.print(PAC);
}

/**********************************************************
**Name:     sigfox ping
**Function: ping wisol device and print out the ack
**Input:    none
**Output:   none
**note:
**********************************************************/
void sigfoxPing(void) {
  //  Serial.println("BP4");
  //testing communication to wisol Module (Should retrun "OK" on serial line) ping device
  Sigfox.print(sfx_ping);
  delay(50);
  while (Sigfox.available() > 0) {
    Serial.print((char)Sigfox.read());
    Serial.print("\r");
  }
}
void sigfoxPacketSetup() {
  txPacketTime[0] = 1;
  txPacketTime[1] = 3;//3 for single and multi
  txPacketTime[2] = 3;//3 for single and multi
  txPacketTime[3] = 3;//3 for single and multi
  txPacketTime[4] = 3;//3 for single and multi

#ifdef ENERGY_MONITOR_APPLICATION
  txPacketTime[0] = 1;
#endif
}
