#include "Definition.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <DallasTemperature.h>
#include <OneWire.h>
#define  firmwareVersion                1.3


/*comment out unsed application, only one application can run at the time*/
//#define WATER_APPLICATION
//#define TEMPERATURE_APPLICATION
#define ELECTRICITY_APPLICATION
//#define AIR_QUALITY
//#define ENERGY_MONITOR_APPLICATION


/*Device running mode */
#define LIVE_MODE        //Normal operation cloud transmit
//#define DEBUG_MODE     //Debug mode does not transmit to cloud, prints machine state and out packet on serial port

#ifdef TEMPERATURE_APPLICATION

OneWire  tempSensor_One(tempSensor_One_ = 180PIN);                                 //Data wire is plugged into pin 7
OneWire  tempSensor_Two(tempSensor_Two_PIN);
OneWire  tempSensor_Three(tempSensor_Three_PIN);
OneWire  tempSensor_Four(tempSensor_Four_PIN);
DallasTemperature sensor_One(&tempSensor_One);                                     // Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensor_Two(&tempSensor_Two);
DallasTemperature sensor_Three(&tempSensor_Three);
DallasTemperature sensor_Four(&tempSensor_Four);
DeviceAddress sensor1, sensor2, sensor3, sensor4;
#endif

#ifdef AIR_QUALITY
Adafruit_BME680 bme; //Air quality
#endif

#ifdef ELECTRICITY_APPLICATION
#define RS485Enable     8
#define maxMeters          8
uint8_t meters = 8;
SoftwareSerial RS485Serial(9, 17); // RX, TX
const uint8_t depth                               = 12;               // Maximum number of sensors. Will be used to setup the arrays used to read meters
typedef struct {
  int8_t MID;
  uint16_t preAmble;
} SensorDetails;
SensorDetails sensors[depth];
unsigned int sensorSctCounter[maxMeters];
#define numSensorDates    2
long sensorValues[depth];
long historicalSensorValues[2];
long sensorDates[numSensorDates];
uint8_t currentMeter                              = 0;

#endif

uint8_t txTime = 15; //15min for single 2hr for multi need to changes to hours
int powerState = 0;
int powerPin = A6;

bool devcieWakeup = false;

void setup() {

  Wire.begin();
  Serial.begin(115200);                //debbug print out
  Sigfox.begin(9600);
  led.begin(5, 10);   //red, blue
  setupBQ27441();

  RTC_Init();                              //Initialize RTC module with compile time
  deviceTrasmit_time(minutes, txTime);     //defines device transmitting time (seconds/minutes/hours)  max 255
  //donwlinkTimer(hours, 1);               //
  //rtc.timeAlarm(8, 9);                   //daily message time 08:09 AM

  //refer to RTClight library example "Multi_interrupt" for how alarm is setup
  //rtc.timeAlarm(8,30);                  //daily message time interrupt 08:30
  //rtc.dailyAlarm(25,12,45);              //Monthly message every 25th 12:45

  pinMode(sigfoxWake_PIN, OUTPUT);
  pinMode(waterSensor_PIN, INPUT_PULLUP);
  pinMode(rtc_INT_PIN, INPUT_PULLUP);
//  pinMode(sigfoxReset_PIN, OUTPUT);
  //  pinMode(batteryDisable_PIN, OUTPUT);

  sigfox.state = ping;                 //start device by pinging wisol module to check if working or not
  delay(500);

  downlink.state = dataRequest;

#ifdef DEBUG_MODE:
  Serial.println("DEBUG");
#endif

#ifdef AIR_QUALITY
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320°C for 150 ms
  // Now run the sensor to normalise the readings, then use combination of relative humidity and gas resistance to estimate indoor air quality as a percentage.
  // The sensor takes ~30-mins to fully stabilise
  GetGasReference();
#endif

#ifdef ELECTRICITY_APPLICATION
  pinMode(RS485Enable, OUTPUT);
  RS485Serial.begin(9600);
  Map_Sensors();
  Sigfox.listen();
  RS485Serial.listen();
  RS485Serial.println("ok");
  Sigfox.listen();
#endif
  led.blinkTime(BLUE, 3, 300); //blink red three times if no meter found
}


void loop() {

  switch (APP_STATE()) {
    case SYSTEM_INITIALIZE:
      Initialize();
      break;

    case SFX_SLEEP:
      Serial.println("Sleep");
      sigfox.state = DeepSleep;
      alarmTriggered = false;
      pulseInterruptFlag = false;
      APP_STATE()  = CPU_SLEEP;
      break;

    case CPU_SLEEP:
      fuelGuageHibernate();     //hibernate fuel guage before  sleep to reduce current consumption
      sleepMode();
      break;

    case CPU_WAKE_UP:

      if (ALARM_INTERRUPT) { //daily message timer
        dailyPacket = true;
        rtc.INTF_CLR(AF);                   //clear rtc alarm interrupt flag
      } else if (TIMER_A_INTERRUPT) {       //run sensor timer
        rtcWakeupCounter++;
        rtc.INTF_CLR(CTAF);                //clear timer A countdown interrupt flag
      }/* else if (TIMER_B_INTERRUPT) {
        rtc.INTF_CLR(CTBF);        //clear timer B countdown interrupt flag ujnder ground test
        APP_STATE() = RQST_DWNLK;
        led.blink(MAGENTA, 1000);
        return;
      }*/else {
        APP_STATE() = RQST_DWNLK;
        led.blink(MAGENTA, 1000);
        return;
      }
      BatteryState();
      devcieWakeup = true;
      APP_STATE() = RUN_SENSOR_CODE;
      return;

      break;

    case RUN_SENSOR_CODE:
      packetToTX = 0;
      CPU_WakeUp();
      //Serial.println("Run Sesnor Code");
      break;

    case CPU_DATA_STORE:
      dataStore();
      APP_STATE() = SFX_CHECK_SEND;
      break;

    case SFX_CHECK_SEND:
      checkSigfoxSend();
      break;

    case SFX_SEND_MSG:
      //Serial.println("SFX_SEND_MSG");
      if (transmissionStatus == START) {
        sigfox.state = wakeUp;
        //Serial.println("BP_SFX_START");
        transmissionStatus = INPROGRESS;
      } else if (transmissionStatus == INPROGRESS) {

      } else if (transmissionStatus == COMPLETE) {
        sigfox.state = delayBetweenSends;       //update wisol module state to standby
        //Serial.println("BP_SFX_COMPLEET");

      } else if (transmissionStatus == FINALISED) {
        //Serial.println("BP_SFX_FINALISED");
        APP_STATE()  =  SFX_CHECK_SEND;
        transmissionStatus = START;
        sigfox.state = standBy;
      }

      break;

    case RQST_DWNLK:
      requestDownlink(packetRequest);
      break;
  }

  wisolRoutine();
}
void checkSigfoxSend() {
  if (packetToTX < numSigfoxPackets) {
    Serial.print("Pkt: ");
    Serial.println(packetToTX);
    Serial.print("rtcCount: ");
    Serial.print(rtcWakeupCounter);
    Serial.print(" Result: ");
    Serial.println(rtcWakeupCounter % txPacketTime[packetToTX]);
    if (rtcWakeupCounter % txPacketTime[packetToTX] == 0) { // change to teh moduls of
      APP_STATE()  = SFX_SEND_MSG;
      //      Serial.println("BP_SFX");
    }
  } else {
    APP_STATE() = RUN_SENSOR_CODE;
    //Serial.println("Next Meter");
    //alarmTriggered = false;
    //pulseInterruptFlag = false;
    //APP_STATE()  = SFX_SLEEP;
  }
  packetToTX++;
}
/**********************************************************
**Name:     Initialize
**Function: Wait for sigfox module to complete Initialization before going to sleep
**Input:    none
**Output:   print out voltage battery level
**note:     transmit time one byte max (255) define in decimal
**********************************************************/
void Initialize(void) {
  if (sfx_Init_Complete == true) {       //set sigfox to sleep after initialized successful
    BatteryState();
    APP_STATE() = CPU_SLEEP;
  }
}

/**********************************************************
**Name:     data strore
**Function: stores sensor reading every transmit time/3
**Input:    none
**Output:   none
**note:
**********************************************************/
void dataStore(void) {
  switch (dataStoreCount) {

    case 1:

#ifdef AIR_QUALITY
      airQuality(0);
#endif

#ifdef WATER_APPLICATION
      dataStrore[0].flowRate = flowRate;
      flowRate = 0;
#endif

      dataStoreCount++;
      break;

    case 2:

#ifdef AIR_QUALITY
      airQuality(1);
#endif

#ifdef TEMPERATURE_APPLICATION
      DS18x_Temp_dataStore(1);
#endif

#ifdef WATER_APPLICATION
      dataStrore[1].flowRate = flowRate;
      flowRate = 0;
#endif
      dataStoreCount++;
      break;

    case 3:

#ifdef AIR_QUALITY
      airQuality(2);
#endif

#ifdef TEMPERATURE_APPLICATION
      DS18x_Temp_dataStore(2);
#endif

#ifdef WATER_APPLICATION
      dataStrore[2].flowRate = flowRate;
      flowRate = 0;
#endif
      dataStoreMax = true;
#ifdef DEBUG_MODE
      BatteryState();
#endif;
      break;
  }
}
/**********************************************************
**Name:     sleepMode
**Function: set CPU to sleep mode to reduce battery consuption and await pulse interrupt to wake up
**Input:    none
**Output:   none
**note:     two different can awake micro, alarm by the rtc or a falling edge pulse on INT1
**********************************************************/
void sleepMode(void) {

  if (alarmTriggered == true || pulseInterruptFlag == true) {
    ADCSRA |= (1 <<  ADEN);         //enable ADC
    APP_STATE()  = CPU_WAKE_UP;     //change application state to "CPU_WAKE_UP" after CPU has awaken from deep sleep
    sigfox.state = standBy;         //sigfox does nothing at wake up untill instructed to

    return;
  }
  //  sigfox.state = standBy;         //sigfox does nothing at wake up untill instructed to
  CPU_Sleep();
}

/**********************************************************
**Name:     CPU Sleep
**Function: set CPU to sleep mode to reduce battery consuption and await pulse interrupt to wake up
**Input:    none
**Output:   10uA expected current while asleep
**note:     two different interrupt event can awake processor:
            a falling edge pulse form rtc INT or a falling edge pulse on INT1
**********************************************************/
void CPU_Sleep(void) {
  pinMode(sigfoxReset_PIN, INPUT);
  pinMode(sigfoxWake_PIN, INPUT);                            //change sigfox wake and reset pin to input(this reduces current concemption)
#ifdef DEBUG_MODE
  Serial.println("CPU SLP");
#endif

  delay(10);
  ADCSRA &= ~(1 << ADEN);                                    // disable ADC;                                              // disable ADC
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  noInterrupts ();                                           //Do not interrupt before we go to sleep, or the ISR will detach interrupts and we won't wake.
  attachInterrupt (TIME_INT0, alarmMatch, FALLING);          //will be called when pin D2 goes low
  EIFR = bit (INTF0);                                        //clear flag for interrupt 0

#ifdef WATER_APPLICATION
  attachInterrupt (SENSOR_INT1, pulseAwake, FALLING);        //will be called when pin D3 goes Low
  EIFR = bit (INTF1);                                        //clear flag for interrupt 1
#endif
  MCUCR = bit (BODS) | bit (BODSE);                          //turn off brown-out enable in software, BODS must be set to one and BODSE must be set to zero within four clock cycles
  MCUCR = bit (BODS);                                        //The BODS bit is automatically cleared after three clock cycles

  /*We are guaranteed that the sleep_cpu call will be done
    as the processor executes the next instruction after
    interrupts are turned on.
  */
  interrupts ();  // Enable interrupt after all setting above is done, one cycle
  sleep_cpu ();   //call sleep iteruction,one cycle*/
}

/**********************************************************
**Name:     pulse Awake
**Function: awake the cpu from sleep,
**Input:    none
**Output:   none
**note:     external interrupt INT1 can be used for any type of sensor enable to generate a falling/rising edge pulse
**********************************************************/
void pulseAwake(void) {
  pulseInterruptFlag = true;
  //Serial.println("Trigger");
  sleep_disable();
  //detachInterrupt (SENSOR_INT1);       // precautionary while we do other stuff disable interrupts
  detachInterrupt (TIME_INT0);
}

/**********************************************************
**Name:     alarm Match
**Function: rtc interrupt pulse to awake cpu from sleep
**Input:    none
**Output:   none
**note:     external interrupt INT0 is reserved(can not be used for sensors but rtc)
**********************************************************/
void alarmMatch(void) {
  alarmTriggered = true;
  sleep_disable();
  pinMode(sigfoxWake_PIN, OUTPUT);          //enable sigfox wakeup pin
  detachInterrupt (TIME_INT0);              //precautionary while we do other stuff disable interrupts
  Serial.println("CPU wkp");
}

/**********************************************************
**Name:     I2C scan
**Function: general purpose function to read and print I2C device address on the bus
**Input:    none
**Output:   I2C device address on the bus
**note:
**********************************************************/
uint8_t I2Cscan(uint8_t index) {
  uint8_t deviceCounter;
  deviceCounter = 0;
  for (int i2c_addr = 0; i2c_addr < 0x7F; i2c_addr++) {
    Wire.beginTransmission(i2c_addr);
    if (!Wire.endTransmission()) {
      deviceCounter++;
      I2C_address[deviceCounter] = i2c_addr;
      //Serial.print("I2C Add:");
      //Serial.println(i2c_addr, HEX);
      //Serial.println(deviceCounter);
    }
    Wire.endTransmission();
  }
  return  I2C_address[index];
}

/**********************************************************
**Name:     deviceTrasmit time
**Function: set device cloud transmit time
**Input:    time uint, tikme
**Output:   none
**note:
**********************************************************/
uint8_t deviceTrasmit_time(uint8_t transTimeUnit, uint8_t Time) {
  uint8_t timeUnit[3] = {COUNT_DOWN_HOUR, COUNT_DOWN_MINUTE, COUNT_DOWN_SECOND};
  rtc.countDown_Enable_TMRA(timeUnit[transTimeUnit], Time);
}

void donwlinkTimer(uint8_t TimeUnit, uint8_t Time) {
  uint8_t timeUnit[3] = {COUNT_DOWN_HOUR, COUNT_DOWN_MINUTE, COUNT_DOWN_SECOND};
  rtc.countDown_Enable_TMRB(timeUnit[TimeUnit], Time);
}
