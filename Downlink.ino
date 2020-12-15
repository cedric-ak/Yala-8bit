void requestDownlink(char *indentifier) {

  switch (downlink.state) {

    case dataRequest:
      sigfoxPing();
      rxRequest(indentifier);
      break;

    case dataReceive:
      rxMsg();
      break;

    case dataProcess:
      rxProcess();
      break;

    case Standby:
      downlink.state = dataRequest;
      APP_STATE() = SFX_SLEEP;
      break;
  }

}

void rxRequest(char *msgIdentifier) {
  Sigfox.print(sfx_sendPacket + msgIdentifier); //downlink request
  RX_period = millis();
  Serial.println("Downlink requested");
  downlink.state = dataReceive;
}

void rxMsg(void) {
  while (Sigfox.available() > 0) {
    msgRead = Sigfox.read();
    downlink_msg += msgRead;
    if (downlink_msg.length() >= 32) {      //if data receive complete
      Serial.print(downlink_msg);           //dataReceive eg:RX=30 33 33 33 5F 5A 35 6C. "RX" at position array 4 and 5
      downlink.state = dataProcess;
    }
  }
  if (millis() - RX_period >  downlinkTimeout) {     //if waiting period timeout retry transmit a new request
    TXattempt++;                                     //increment downlink request attemp
    if (TXattempt < maxAttempt) {
      downlink.state = dataRequest;
      Serial.println("\ndownlink timeout");
      led.blinkTime(RED, 2, 300);                
    } else {
      TXattempt = 0;
      Serial.println("downlink Request failed!!");
      downlink.state = Standby;                     //if success print received msg and process
      led.blink(RED, 1000);                  
    }
  }
}

void rxProcess(void) {

  String inString = "";
  String Data = "";

  for (int i = 7; i < 9; i++) {   //decode the first byte of RX msg (identifier)
    int inChar = downlink_msg.charAt(i);
    if (isDigit(inChar)) {
      inString += (char)inChar;
    }
  }

  int identifier = inString.toInt(); //convert string to integer
  inString = "";                     //clear string

  switch (identifier) {

    case 1://TX time update
      uint8_t  timeUnit = rxRead(2, 1);             //byte position 2, number of bytes to read = 1. Time unit: 0 = hour, 1 = minute, 2 = seconds
      if (timeUnit == 1 )
        Serial.println("Minutes");
      else if (timeUnit == 0)
        Serial.println("Hours");
      else if (timeUnit == 2)
        Serial.println("Seconds");
      Serial.println(timeUnit);
      uint16_t timeUpdate    = rxRead(3, 2);             //byte position 3-4, number of bytes to read = 2
      Serial.println(timeUpdate, DEC);
      deviceTrasmit_time(timeUnit, timeUpdate);        
      downlink.state = Standby;
      break;

    case 2: break;
    case 3: break;
    case 4: break;
    case 5: break;
    case 6: break;
    case 7: break;

  }
  downlink.state = Standby;
  downlink_msg = "";
  led.blink(BLUE, 1000);  
}

uint16_t rxRead(uint8_t byteIndex, uint8_t numByte) {
  String inString = "";

  int bytePosition = 7 + (2 * byteIndex);

  for (int i = bytePosition; i < bytePosition + 2; i++) { //read byte from RX msg
    int inChar = downlink_msg.charAt(i);
    if (isDigit(inChar)) {
      inString += (char)inChar;
    }
  }


  if (!(numByte % 2)) {
    for (int i = bytePosition + 2; i < bytePosition + 5; i++) { //read RX msg
      int inChar = downlink_msg.charAt(i);
      if (isDigit(inChar)) {
        inString += (char)inChar;
      }
    }
  }
  //uint8_t timeUnit = inString.toInt();             //time unit 0 = hour, 1 minute, 2 = seconds
  int data = inString.toInt();                       //convert the two byte data to int
  return data;
}
