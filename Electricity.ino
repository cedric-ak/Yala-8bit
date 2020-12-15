/**********************************************************
**Name:     CPU WakeUp
**Function: CPU wake up routine excute electricity process
**Input:    none
**Output:   none
**note:     this function is define multiple times for different applicatons
**********************************************************/
#ifdef ELECTRICITY_APPLICATION


#define CodeVersion                               99999

#include "I2C_Techsitter.h"
//#include <SoftwareSerial.h>
//#include <SPI.h>                                                        //Used to communicate with the Radio
//#include "Brain_Node_Definitions.h"                                       //Overall definitions file
#include <SN74HC4851.h>
///////////////////////////////////////////////////////////////////////////////
// USER CONFIG SETTINGS
////////////////////////////////////////////////////////////////////////////////
#define sensorsToignore                           3
#define maxNoMeterData                            20
////////////////////////////////////////////////////////////////////////////////
// USER CONFIG SETTINGS NOT CHANGED OFTEN
////////////////////////////////////////////////////////////////////////////////
#define startupDelay                              1000L               //Delay the program on startup to see what's on the serial
#define Sprintln(a)                               (Serial.println(a)) //This definition allows for serial printing to be turned off in bulk by removing the definition
#define Sprint(b)                                 (Serial.print(b))   //This definition allows for serial printing to be turned off in bulk by removing the definition
#define S_println(a)                              (Serial.println(a)) //This definition allows for serial printing to be turned off in bulk by removing the definition
#define S_print(b)                                (Serial.print(b))   //This definition allows for serial printing to be turned off in bulk by removing the definition
#define SERIAL_BAUD                               115200              //Serial speed for debugging
////////////////////////////////////////////////////////////////////////////////
// OTHER CONFIG SETTINGS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// READINGS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// RS485 Iskra
////////////////////////////////////////////////////////////////////////////////

char receivedChars[50];
int i = 0;
uint8_t portNumber = 0;     //start RS485 reading from port 1.
unsigned long lastMeterCommTime  = 0;
unsigned long meterCommTime;
unsigned long lastPortTime = 0;
unsigned long portTimeout  = 2000;
uint8_t meterTimeoutCount = 0;
uint8_t meterCount = 0;
String incoming = "";
bool firstTimeStore = true;
bool meterDoneReading = false;
SN74HC4851 my_mux(11, 12, 13);
////////////////////////////////////////////////////////////////////////////////
bool mt880;
bool intialiseRS485 = false;
bool timeToStore = false;
TechSitter_I2C I2C;
enum ElecMeterStatus : uint8_t { EM_IDLE = 0, EM_SD_INTIALSE_METER = 1, EM_WAIT_INTIAL_RESPONSE = 2, EM_REC_INTIAL_RESPONSE = 3, EM_RETRY = 4, EM_SD_ACK_METER = 5, EM_WAIT_DATA = 6, EM_REC_LINE = 7, EM_REC_MATCH_DATA = 8, EM_REC_TIME_STORE = 9, EM_REC_ENDFILE = 10, EM_SEND_SIGFOX = 11, EM_REQUEST_NEXT = 12, EM_READCOMPLETE = 13, EM_DISPLAYCOMPLETE = 14, EM_DISPLAYMETERCOUNT = 15};
ElecMeterStatus elecMeterStatus;
uint8_t oldstatus = 255;
bool firstTime = true;
uint16_t NumResets;
long lastPrintState = 0;
#define  PrintStateTime 30000
long startEMstatus = 0;
long  EMTimeout = 30000;
long startReset = 0;
#define timeToWaitBeforeReset        10000
bool remoteReset = false;
bool firstTimeReset = true;
void transitionState(ElecMeterStatus newStatus);                  //added
bool StartRead = true;
long RGBStartEndTime = 1500;
uint8_t meterNodata[maxMeters];
bool meterSeen[maxMeters];
unsigned long presslength, pressedlength = 0;
extern LED led;
bool continueMeterRead = false;
int sendRetries = 0;
#define maxRetries    2  //(3 times including the initial attempt)

void CPU_WakeUp(void) {

  //if (!i2cScanComplete) {
  for (int device = 0; device < 2; device++) {         //check which expansion board is connected
    char seen_I2C_address = I2Cscan(device);
    Serial.print("Seen: ");
    Serial.println(seen_I2C_address, HEX);
    if (seen_I2C_address ==  singleChannelRS485_Add) {
      mt880 = true;
      meters = 1;
      Serial.print("Mtr: ");
      Serial.println(meters);
      device = 2;
    } else {             //if disconnetor
      mt880 = false;
      meters = 8;
      Serial.print("Mtr: ");
      Serial.println(meters);
    }
  }
  //i2cScanComplete = true;
  //}

  lastMeterCommTime  = 0;
  meterCommTime = 30000;
  timeToStore = false;
  receivedChars[49] = char(10);
  //Serial.print("ElectState1 ");
  //Serial.println(elecMeterStatus);
  //Serial.println(continueMeterRead);
  if (continueMeterRead == true) {
    elecMeterStatus = EM_REQUEST_NEXT;
  } else {
    elecMeterStatus = EM_IDLE;
  }
  //elecMeterStatus = EM_SD_INTIALSE_METER;
  //Serial.print("ElectState2 ");
  //Serial.println(elecMeterStatus);

  while (elecMeterStatus != EM_SEND_SIGFOX && elecMeterStatus != EM_DISPLAYMETERCOUNT) {
    runMeterReading();
    //Serial.flush();
  }
  //RS485Serial.flush();
  if (elecMeterStatus == EM_DISPLAYMETERCOUNT) {
    APP_STATE() = SFX_SLEEP;
  } else {
    APP_STATE()  = CPU_DATA_STORE;
  }

  //Serial.print(APP_STATE());
  //elecMeterStatus = EM_SD_INTIALSE_METER;
}

void runPrintState() {
  if (millis() - lastPrintState > PrintStateTime) {
    lastPrintState = millis();
    //Sprint(F("St:"));
    //Sprintln(elecMeterStatus);
  }
}


//Check to see that the board passed the testing in production

//////////////////////////////////
//used to print out the details so the Node can be imported into Azk
void printDetails() {
  Serial.println(F("@CS"));
  Serial.print(F("DS:"));
  Serial.println(F(""));
  Serial.print(F("FI:"));
  Serial.println(CodeVersion);
  Serial.println(F("@CE"));
}

////////////////////////////////////////////////////////////////////////////////
// RS485 Iskra
////////////////////////////////////////////////////////////////////////////////
void transitionState(ElecMeterStatus newStatus) {
  elecMeterStatus  = newStatus;
  startEMstatus = millis();
}
void runMeterReading() {
  switch (elecMeterStatus) {
    case EM_IDLE:

      checkprintState();
      led.blink(MAGENTA, 1500);    //indicate begin of meter read
      if (checkForReadStart() == true) {
        transitionState(EM_SD_INTIALSE_METER);
      }
      break;
    case EM_SD_INTIALSE_METER:
      historicalSensorValues[0] = sensorValues[0];
      //Serial.print("historical 20180 ");
      //Serial.println(historicalSensorValues[0]);

      Sigfox.listen();
      RS485Serial.listen();
      checkRS485Buffer();
      checkprintState();
      Sprintln(portNumber);
      if (mt880 == false) {
        RS485Port(portNumber);
      }
      ackMeterDetails();
      startMeterRequest();
      transitionState(EM_WAIT_INTIAL_RESPONSE);
      break;
    case EM_WAIT_INTIAL_RESPONSE:
      checkprintState();
      checkSerial();
      break;
    case EM_RETRY:
      retrySend();
      break;
    case EM_REC_INTIAL_RESPONSE:
      checkprintState();
      meterNodata[portNumber] = 0;
      meterSeen[portNumber] = 1;
      //Serial.println("Seen");
      //Serial.println(meterSeen[portNumber]);
      transitionState(EM_SD_ACK_METER);
      break;
    case EM_SD_ACK_METER:
      checkprintState();
      //check to see if the meter has responded to our first intialisation request
      ackMeterDetails();
      transitionState(EM_WAIT_DATA);
      break;
    case EM_WAIT_DATA:
      checkprintState();
      checkSerial();
      break;
    case EM_REC_LINE:
      checkprintState();
      checkSerial();
      break;
    case EM_REC_MATCH_DATA:
      checkprintState();
      //won't get here for now
      break;
    case EM_REC_TIME_STORE:
      checkprintState();
      //won't get here for now
      break;
    case EM_REC_ENDFILE:
      checkprintState();
      sendRetries = 0;
      printObis();
      led.blink(BLUE, 300);  //I spoke to the meter
      //Serial.println(F("G"));
      //rgbBoard.StopRGBIdent(true);
      //changeRGB(greenRGB, RGB_METER); //I spoke to the meter
      continueMeterRead = true;
      transitionState(EM_SEND_SIGFOX);
      break;
    case EM_SEND_SIGFOX:

      //transitionState(EM_REQUEST_NEXT);
      break;
    case EM_REQUEST_NEXT:
      checkprintState();
      firstTimeStore = true;
      readMeter();
      break;
    case EM_READCOMPLETE:
      checkprintState();
      checkNoMeterTranistitionDisplayComplete();
      break;
    case EM_DISPLAYCOMPLETE:
      checkprintState();
      continueMeterRead = false;
      checkRGBCompleteTransitionDisplayMeterCount();
      break;
    case EM_DISPLAYMETERCOUNT:
      //printOutAllValues();
      displayRGBMeters();

      transitionState(EM_IDLE);
      break;
    default:
      // statements
      break;
  }
}
void retrySend() {
  if (sendRetries < maxRetries) {
    sendRetries++;
    transitionState(EM_SD_INTIALSE_METER);
    Serial.println("Resnd");
  } else {
    Serial.println("Max Resnd");
    sendRetries = 0;
    meterTimeoutCount++;
    led.blink(RED, 300); //did not find a meter on port x
    //Serial.print("meterTimeoutCount");
    //Serial.println(meterTimeoutCount);
    //Sprintln(F("R"));
    //rgbBoard.StopRGBIdent(true);
    //changeRGB(redRGB, RGB_METER); //I spoke to the meter
    if (meterSeen[portNumber] == 1) {
      //Serial.println("Missing:");
      //Serial.println(portNumber);
      meterNodata[portNumber]++;
    }
    transitionState(EM_REQUEST_NEXT);
  }

}
void displayRGBMeters() {
  if (meterCount != 0) {
    //rgbBoard.StopRGBIdent(true);
    //Sprintln(F("B"));
    //changeRGB(blueRGB, RGB_METER);
    //rgbBoard.ChangeFlashTime(meterCount);
    //    led.blinkTime(BLUE,meterCount,500);
  }
  Sprintln(F("MtNo"));
  for (int i = 0; i < meters; i++) {
    Sprintln(meterNodata[i]);
    if (meterNodata[i] > maxNoMeterData) {
      Sprintln(F("Greater than max"));
      remoteReset = true;
    }
  }

}
void checkRGBCompleteTransitionDisplayMeterCount() {
  if (millis() - startEMstatus > RGBStartEndTime) {
    Sprint(F("M_C:"));
    Sprintln(meterCount);
    transitionState(EM_DISPLAYMETERCOUNT);
  }
}
void checkNoMeterTranistitionDisplayComplete() {
  if (millis() - startEMstatus > 1000) {
    if (meterCount == 0) {                  //blink red if no meter is detected
      led.blinkTime(RED, 3, 300); //blink red three times if no meter found
      meterDoneReading  = false;
    } else  {
      //rgbBoard.StopRGBIdent(true);
      //Sprintln(F("B"));
      //changeRGB(user3RGB, RGB_METER);
      //led.blink(MAGENTA, 1500);
    }
    transitionState(EM_DISPLAYCOMPLETE);
  }
}
bool checkForReadStart() {


  //if (millis() - lastMeterCommTime > meterCommTime) {

  lastMeterCommTime = millis();
  lastPortTime = millis();
  meterTimeoutCount = 0;
  portNumber = 0;
  StartRead = true;
  return true;
  //} else {
  //  return false;
  //}

}
/*
  void displayRGBforReadStart() {
  //check the time to see if we should request electricity values from the meter
  if ((millis() - lastMeterCommTime > meterCommTime - RGBStartEndTime) && StartRead == true) {
    StartRead = false;
    //rgbBoard.StopRGBIdent(true);
    //Sprintln(F("Y"));
    //changeRGB(user3RGB, RGB_METER);
  }
  }
*/
void checkRS485Buffer() {
  if (RS485Serial.available()) {
    Serial.println(F("!!"));
  }
  while (RS485Serial.available()) {
    Serial.print(char(RS485Serial.read()));

  }
  Serial.println(F(""));
  Serial.println(F("@@"));
}
void checkprintState() {

  if (oldstatus != elecMeterStatus) {
    Sprint(F("S:"));
    Sprintln(elecMeterStatus);
    oldstatus = elecMeterStatus;
  }

}
void checkSerial() {
  //If data is available on RS485 then process it
  while (RS485Serial.available() ) {
    processSerial();
  }
  if (millis() - lastPortTime > portTimeout) {
    lastPortTime = millis();
    transitionState(EM_RETRY);
  }
}
void processSerial() {
  //Read the incoming data into  inChar
  char inChar = (char)RS485Serial.read();
  //Since we are using 8N1 on software serial and the meter transmits at 7E1 we need to AND with 0x7F
  inChar &= 0x7F;
  Sprint(inChar);
  switch (inChar) {
    case 10://Data Link Escape
      if (intialiseRS485 == true) {
        intialiseRS485 = false;
        //Sprintln(F("Mt"));
        transitionState(EM_REC_INTIAL_RESPONSE);
      } else {
        for ( int i = 0; i < sizeof(receivedChars);  ++i ) {
          receivedChars[i] = (char)0;
        }
        incoming += inChar;
        incoming.toCharArray(receivedChars, 50);
        processLine();
      }
      break;
    case 3:
      //End of Text
      transitionState(EM_REC_ENDFILE);
      break;

    case 2:
      //We have started to receive the OBIS data
      //Serial.println("Found SOT");
      incoming = "";
      i = 0;
      break;
    case 0:
      break;
    default:
      //As long as we have not receive a CR or LF add to the buffer
      incoming += inChar;
      //receivedChars[i] = inChar;
      //receivedChars = incoming;
      //Serial.print("BP!!");
      //Serial.print(i);
      //Serial.print(" ");
      //Serial.println(receivedChars);
      //Serial.println(incoming);
      //Serial.println("");
      //Serial.println(inChar, HEX);
      //Serial.println(incoming);
      i++;
      break;
  }

}
void processLine() {
  //When we receive a line of data then process it
  transitionState(EM_REC_LINE);
  incoming = "";
  i = 0;
  //Serial.println("Receiving");
  //Serial.println(receivedChars);
  //Determine where the '*' is in the data. If the last * is greater than 10 then we are receiving units in the data
  int lastOpeningBracket = String(receivedChars).lastIndexOf('*');
  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(receivedChars, ":");     // get the first part - the string
  //To get the Obis code we remove everything before the ':'
  //Serial.println(receivedChars);
  //The line below gets the Obis Code
  strtokIndx = strtok(NULL, "("); // this continues where the previous call left off
  ////Serial.print(strtokIndx);
  ////Serial.print("\t\t");
  String receivedObis = strtokIndx;
  //Serial.println("Obis");
  //Serial.println(receivedObis);
  int starPosition = receivedObis.lastIndexOf('*');
  //Serial.println(String(starPosition));
  if (starPosition > 0) {
    receivedObis = receivedObis.substring(0, starPosition);
  }
  //Serial.println(receivedObis);
  //Serial.println(locationOfStar);
  bool noUnits = false;
  String receivedValue = "";
  //Determine if we are receiving units. If yes then for the data we only want everything up to the '*'
  if (lastOpeningBracket > 10) {
    strtokIndx = strtok(NULL, "*"); // this continues where the previous call left off
    ////Serial.print(strtokIndx);
    ////Serial.print("\t\t");
    receivedValue = strtokIndx;
    //Serial.println("Value is ");
    //Serial.println(receivedValue);
  } else {
    noUnits = true;
  }
  strtokIndx = strtok(NULL, ")"); // this continues where the previous call left off
  ////Serial.print(strtokIndx);
  ////Serial.print("\t\t\t");
  //If there were no units then the data up to the ')' is the value
  if (noUnits == true) {
    ////Serial.print("NA");
    ////Serial.print("\t\t");
    receivedValue = strtokIndx;
    //Serial.println(strtokIndx);
  }
  strtokIndx = strtok(NULL, ")"); // this continues where the previous call left off
  String receivedValue2 = strtokIndx;
  //Serial.println(receivedValue2);
  //If there is another ')' then we have received more info about the measurement such as the date when the max demand occured
  //Serial.println(receivedValue2[0]);
  char receivedDate[10];
  if (receivedValue2[0] == '(') {
    receivedValue2 = receivedValue2.substring(1, 11);
    receivedValue2.toCharArray(receivedDate, 11);
    //Serial.println("Date incoming");
    //Serial.println(receivedDate);
  } else {
    receivedValue2 = "0";
  }
  ////Serial.println("");
  //clear out the receivedchars buffer
  for (int i = 0; i < 50; i++) {
    receivedChars[i] = NULL;
  }
  if (receivedObis != "") {
    //Serial.println("Rec Value is");
    //Serial.println(receivedValue);
    //for (int i = 0; i < receivedValue.length(); i++) {
    //  Serial.println(receivedValue[i]);
    //}
    //Serial.println("Done");
    //Serial.println("Values");
    //Serial.println(String(receivedObis));
    //Serial.println(String(receivedValue));
    //Serial.println("Values");
    checkObis(receivedObis, receivedValue, receivedDate);
    //Serial.println(sensors[0].Value);
  }
}
void ackMeterDetails() {
  RS485WriteAckMeterDetails();
  incoming = "";
}
void startMeterRequest() {
  incoming = "";
  i = 0;
  //Sprintln(F("Int"));
  intialiseRS485 = true;
  RS485WriteZero();
  delay(300);
  RS485WriteStartRequest();
}
uint16_t convertToPreamble(String toConvert) {
  uint16_t toReturn = 0;
  if (toConvert.length() == 5) {
    toReturn = 20000 + (toConvert.substring(0, 1).toInt() * 100) + (toConvert.substring(2, 3).toInt() * 10) + (toConvert.substring(4, 5).toInt() * 1);
  } else if (toConvert.length() == 6) {
    toReturn = 20000 + (toConvert.substring(0, 2).toInt() * 100) + (toConvert.substring(3, 4).toInt() * 10) + (toConvert.substring(5, 6).toInt() * 1);
  } else  {
    toReturn = 0;
  }
  long endCheck = millis();
  return toReturn;
}
void checkObis(String toCheck, String Value, char Date[10]) {
  //  Serial.println("");
  //Sprintln(F("CheckO"));
  //  Sprintln(Date);
  //Sprintln(Value);
  //Sprintln(toCheck);
  uint16_t checkAgainstPreamble = convertToPreamble(toCheck);
  //Sprintln(checkAgainstPreamble);

  uint16_t sensorTimePreAmble;
  uint16_t sensorDatePreAmble;
  for (int i = 0; i < depth; i++) {

    if (sensors[i].preAmble == checkAgainstPreamble) {
      //Sprintln(F("Match"));
      //      Sprintln(F("i is "));
      //      Sprint(i);
      //      Serial.println(String(sensors[i].code));
      //      Serial.println(toCheck);
      //      Serial.println(Value);

      if (toCheck == "13.7.0") {
        Value = Value.substring(0, 1) + Value.substring(2, 5);
        //Serial.println("Check for 13");
        //Serial.println(Value);
      }
      if (toCheck == "0.9.2") {
        //Serial.println("BP4");
        //Serial.println(Value.length());
        if (Value.length() == 7) {
          //This is a specific problem with the MT174 direct connected meter where there is a extra 1
          //We need to remove this 1
          //Serial.println(Value.substring(1));
          Value = Value.substring(1);
        }
        sensorValues[i] = atol(Value.c_str());
        //Serial.println("Here");
        //Serial.println(sensorValues[i][currentMeter]);
        //Serial.println("0.9.2 i is ");
        //Serial.println(i);
      }
      ////Serial.print("Match found for ");
      ////Serial.println(sensors[i].code);
      if (toCheck == "0.9.1") {
        // store the date and time in each call to the meter
        sensorValues[i] = atol(Value.c_str());
        //Serial.println("Here");
        //Serial.println(sensorValues[i][currentMeter]);
        //Serial.println("0.9.1 i is ");
        //Serial.println(i);
        newCheckTime(sensorValues[i]); //sets the flag to store below
      } else if (timeToStore == true) {// else if (timeToStore == true) {

        sensorValues[i] = atol(Value.c_str());
        //Serial.println("BPV");
        //Don't remove the line Below
        Serial.println(atol(Value.c_str()));//Don't remove this line
        //don't remove teh line above
        //Serial.println(toCheck);
        if (toCheck == "0.9.2") {
          //once we have received the date make sure that the date field for 12 gets the date too
          //sensorValues[i][currentMeter].date = calcDateTime();
          //sensorValues[12][currentMeter].date = calcDateTime();
          //sensorValues[14][currentMeter].date = calcDateTime();
          //Serial.println("BPM");
          sensorDates[0] = calcDateTime();
          //Serial.println("0.9.2 i is ");
          //Serial.println(sensorDates[0][currentMeter]);
        }
        //sensorValues[i][currentMeter] = atol(Value.c_str());
        sensorValues[8] = NumResets;
        //Serial.println(atol(Value.c_str()));
        //Serial.println(sensors[i].Value);

        if ( Date[0] != '0' && sensors[i].MID == 51) {
          //Serial.println("Here is a Date");
          //Serial.println(Date);r
          sensorDates[i] = atol(Date);
          //Serial.println(i);
          //Serial.println("Need to store date");
          //sensors[i+1].Value = atol(Date);
          //Serial.println(sensors[i].date);
        } else  {
          //Serial.print("BP:");
          //Serial.print(i);
          //Serial.println(calcDateTime());
          //sensorDates[0][currentMeter] = calcDateTime();
        }
        //if (sensors[i].MID == 51 ) {
        //  sensors[i+1].Value =
        //}
        //Serial.println(sensors[i].Value);
      }
    }

  }

}
void printObis() {
  //S_println("PrObis");
  //Serial.println(sensors[0].Value);
  //adding in here to check actionnode
  //Serial.println("Values for radio");
  for (int i = 0; i < depth; i++) {
    S_print(currentMeter);
    S_print(",");
    S_print(sensors[i].preAmble);
    S_print(",");
    S_print(sensors[i].MID);
    S_print(",");
    S_print(sensorValues[i]);
    S_print(",");
    if (sensors[i].MID == 51) {
      S_println(sensorDates[i]);
    } else  {
      S_println(sensorDates[0]);
    }

  }
  sensorValues[10] = MeterEpoch(calcDateTime());       // meter time and date to epoch since 1/1/2020
  //  Serial.println("State, Phase, Timeout");
  //  Serial.print(tsproto.getState());
  //  Serial.print(" , ");
  //  Serial.print(tsproto.getPhase());
  //  Serial.print(" , ");
  //  Serial.println(timeout);

}
void newCheckTime(float Value) {
  //Serial.println("newCheckTime");
  //int seconds = Value.substring(4,6).toInt();
  //Serial.println(seconds);
  //Serial.println(Value);
  int32_t testInt = (int32_t)Value;
  //Serial.println(testInt);
  //int  seconds = (testInt % 10) + (10*((testInt / 10 ) % 10));
  int minute = ((testInt / 100 ) % 10) + (10 * ((testInt / 1000 ) % 10));
  //Serial. println(seconds);
  //Serial.println(minute);

  //int minute  = Value.substring(2,4).toInt();.preamble


  ////Serial.println(minute);
  //int remainder = minute % 10;
  ////Serial.println(remainder);
  //Serial.println("Minute");
  //Serial.println(minute);
  //Serial.println(firstTimeStore);
  //if ((minute == 0 && firstTimeStore == true) || (minute == 15 && firstTimeStore == true) || (minute == 30 && firstTimeStore == true) || (minute == 45 && firstTimeStore == true) ) {
  //Serial.println("BPS");
  timeToStore = true;
  firstTimeStore = false;
  //} else {
  //  timeToStore = false;
  //  firstTimeStore = true;
  //  //S_println("Not storing yet");
  //}
  //printstoreValues();

}

long calcDateTime() {
  //String dateTime = String(sensors[13].Value).substring(0,6);
  //dateTime += String(sensors[12].Value).substring(0,4);
  //Serial.println("BP3");
  //Serial.println(sensorValues[10][currentMeter]);
  //Serial.println(sensorValues[11][currentMeter]);
  long test = (sensorValues[11] * 10000) + sensorValues[10] / 100;
  //Serial.println("Dt");
  //Serial.println(String(test));
  return test;
}

void RS485WriteAckMeterDetails() {
  //Once we get back the Meter details we ack these to get the meter to send through the Obis data
  //MCP_Write(0x01);
  //Serial.println("-------ACK------");
  delay(100);
  digitalWrite(RS485Enable, HIGH);
  digitalWrite(17, HIGH);
  delayMicroseconds(104);

  RS485Serial.write(0x06);
  RS485Serial.write(0x30);
  RS485Serial.write(0x35);
  RS485Serial.write(0x30);
  RS485Serial.write(0x8D);
  RS485Serial.write(0x0A);
  //delay(1);
  delayMicroseconds(104);
  //MCP_Write(0x00);
  digitalWrite(RS485Enable, LOW);
}
void RS485WriteStartRequest() {
  //The request for getting the Obis code data is
  //  /  = 2FH, we are sending via 8N1 to a device expecting 7E1 so this becomes 0xAF
  //  ?  = 3FH, we are sending via 8N1 to a device expecting 7E1 so this stays 0x3F
  //  !  = 21H, we are sending via 8N1 to a device expecting 7E1 so this stays 0xAF
  //  CR = 0DH, we are sending via 8N1 to a device expecting 7E1 so this becomes 0x8D
  //  LF = 0AH, we are sending via 8N1 to a device expecting 7E1 so this stays 0x0A
  //MCP_Write(0x01);
  digitalWrite(RS485Enable, HIGH);
  digitalWrite(17, HIGH);
  delayMicroseconds(104);
  RS485Serial.write(0xAF);
  RS485Serial.write(0x3F);
  RS485Serial.write(0x21);
  RS485Serial.write(0x8D);
  RS485Serial.write(0x0A);
  //delay(1);
  delayMicroseconds(104);
  //MCP_Write(0x00);
  digitalWrite(RS485Enable, LOW);
}
//void MCP_Write(uint8_t data) {
//  I2C.beginTransmission(0x27);
//  I2C.write(0x0A);
//  I2C.write(data);
//  I2C.endTransmission();
//}
void RS485WriteZero() {
  //The Iskra meter reading software sends out a series of zeros before teh start of the data request
  //MCP_Write(0x01);
  digitalWrite(RS485Enable, HIGH);
  RS485Serial.write( (byte) 0x00 );
  RS485Serial.write( (byte) 0x00 );
  RS485Serial.write( (byte) 0x00 );
  RS485Serial.write( (byte) 0x00 );
  RS485Serial.write( (byte) 0x00 );
  RS485Serial.write( (byte) 0x00 );
  RS485Serial.write( (byte) 0x00 );
  RS485Serial.write( (byte) 0x00 );
  delay(1);
  //MCP_Write(0x00);
  digitalWrite(RS485Enable, LOW);
}

void Map_Sensors() {

  //Be Carfeul - Mid of 51 must start at 1. and All mids of 51 must be in sequence from 1.
  //We are using the position of the position of teh 51 in this array as an index in teh date array
  sensors[0].MID = 31;
  sensors[1].MID = 51;
  sensors[2].MID = 31;
  sensors[3].MID = 1;
  sensors[4].MID = 31;
  sensors[5].MID = 31;
  sensors[6].MID = 31;
  sensors[7].MID = 31;
  sensors[8].MID = 31;

  sensors[9].MID = 31;
  sensors[10].MID = 31;
  sensors[11].MID = 31;

  sensors[0].preAmble = 20180; // Positive Active Energy kWH
  sensors[1].preAmble = 20960;//KVA
  sensors[2].preAmble = 20280;// Negative Active Energy
  sensors[3].preAmble = 20042; //CT Ratio
  sensors[4].preAmble = 20181;//TOU
  sensors[5].preAmble = 20182;//TOU
  sensors[6].preAmble = 20183;//TOU
  //Removed 20184 TOU for quick change to kvarh on 7 Feb 2020
  sensors[7].preAmble = 20380;//kvarh
  sensors[8].preAmble = 20003;//Resets

  sensors[9].preAmble = 20000;//Serial Number
  sensors[10].preAmble = 20091;// Time
  sensors[11].preAmble = 20092;//Date

  //sensors[2].preAmble = 20170;// Positive Inst Power
  //sensors[3].preAmble = 20270;//Neg Inst
  //sensors[6].preAmble = 20045;

  //  for (int i = 0; i < depth; i++) {
  //    sensorValues[i][currentMeter].date = 0;
  //  }
  for (int j = 0; j < meters; j++) {
    sensorSctCounter[meters] = 0;
  }
}

void RS485Port(uint8_t meter) {
  my_mux.channel(meter);
  currentMeter = meter;
}

void readMeter(void) {
  portNumber++; //increment RS485 Port for next meter reading
  //if (mt880 == true ) {
  //  meterTimeoutCount = 7;
  // }
  Serial.println("rMtr");
  Serial.println(portNumber);
  Serial.print("mtrs: ");
  Serial.println(meters);
  if (portNumber > meters - 1) {
    led.blink(MAGENTA, 1500);       //done reading all 8 port
    portNumber = 0 ;
    meterCount = meters - meterTimeoutCount;     // calculate the Number of meters connected
    meterTimeoutCount = 0;                  //clear time out count for next meter reading request
    meterDoneReading = true;
    led.blinkTime(BLUE, meterCount, 300);  //blink a number of meters found
    transitionState(EM_READCOMPLETE);

  } else {
    meterDoneReading = false;
    lastPortTime = millis();                // update last port read to current time
    transitionState(EM_SD_INTIALSE_METER);
  }
}

/*Mtr EPOCH time convert*/
int itoaConv(uint32_t dataIn, int startPosition, int endPosition) {     //convert date and time to string.
  String diN = "";
  String inString = "";
  diN = String(dataIn, DEC);
  for (int i = startPosition; i < endPosition; i++) {
    int inChar = diN.charAt(i);
    if (isDigit(inChar)) {
      inString += (char)inChar;
    }
  }
  int integer = inString.toInt();
  return integer;
}
uint8_t atoiConv(uint32_t dataIn, int bytes) {              //extract year,month,day,hh,mm,ss from string and convert back to integer

  switch (bytes) {
    case 1: return itoaConv(dataIn, 0, 2); break;
    case 2: return itoaConv(dataIn, 2, 4); break;
    case 3: return itoaConv(dataIn, 4, 6); break;
    case 4: return itoaConv(dataIn, 6, 8); break;
    case 5: return itoaConv(dataIn, 8, 10); break;
  }
}

uint32_t MeterEpoch(uint32_t dateTime) {
  return rtc.unixtimeCalc(atoiConv(dateTime, 1), atoiConv(dateTime, 2), atoiConv(dateTime, 3), atoiConv(dateTime, 4), atoiConv(dateTime, 5), 0);//(Y,M,D,HH,MM,SS) Epoch since 1/1/2020
}
#endif
