#include <RTClight.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <SoftwareSerial.h>
#include <LED.h>
SoftwareSerial Sigfox(A0, A1);    //RX,  TX
extern RTC_PCF8523 rtc;
LED led;
//Sigfox transmission
uint8_t rtcWakeupCounter = 0;
#define numSigfoxPackets  5
uint8_t txPacketTime[numSigfoxPackets];
uint8_t packetToTX = 0;
long completedSigfoxTime = 0;
#define sigfoxDelayTime  2000
long startTxTime = 0;
#define TxTimeOut       30000


/************Process State machine Definition*********************/
#define set_State(state)   g_state = state
#define APP_STATE()        g_state

typedef enum {
  SYSTEM_INITIALIZE,
  SFX_SLEEP,
  SFX_CHECK_SEND,
  SFX_SEND_MSG,
  CPU_SLEEP,
  CPU_WAKE_UP,
  RUN_SENSOR_CODE,
  CPU_DATA_STORE,
  RQST_DWNLK,
  SYST_ERROR
} t_Application;

t_Application   APP_STATE() = SYSTEM_INITIALIZE;        //start the code on Normal operation state

typedef enum {          //Wisol module Sigfox machine states
  wakeUp,
  DeepSleep,
  standBy,
  sendPacket,
  checkPacketSent,
  delayBetweenSends,
  sendFrameDone,
  readDevice_ID,
  readDevice_PAC,
  Reset,
  debug,
  ping
} sigfoxState;

typedef struct {
  sigfoxState state;
} sfx;
sfx sigfox;

typedef enum {
  START,
  INPROGRESS,
  COMPLETE,
  FINALISED
} transmissionstatus;

transmissionstatus   transmissionStatus = START;        //start the code on Normal operation state

/****************Sigfox AT-Command************************/
char sfx_ping[]         = "AT\r";        //send a dummy command (response "OK")
char sfx_sleep[]        = "AT$P=1\r";    //put sigfox wisol into sleep mode
char sfx_deepSleep[]    = "AT$P=2\r";    //put sigfox wisol into deep sleep mode
char sfx_sftReset[]     = "AT$P=0\r";    //software reset (settings will be reset to values in flash)
char sfx_ID[]           = "AT$I=10\r";   //request sigfox ID number
char sfx_PAC[]          = "AT$I=11\r";   //request sigfox PAC number
char sfx_getTX_Freq[]   = "AT$IF?\r";    //Get the currently chosen TX frequency.
char sfx_getRX_Freq[]   = "AT$DR?\r";    //Get the currently chosen RX frequency.
char sfx_getTemp[]      = "AT$T?\r";     //Measure internal temperature and return it in 1/10th of a degree Celsius
char sfx_getVolt[]      = "AT$V?";       //Return current voltage and voltage measured during the last transmission in mV.
char sfx_SaveConfig[]   = "AT$WR";       //Write all settings to flash
char sfx_acknowledge[]  = "OK";          //AT-Command acknowledgement response
String sfx_sendPacket   = "AT$SF=";      //send packet
/*************************************************************************/

#define sigfoxReset_PIN      7      //WLS Reset
#define sigfoxWake_PIN       6      //WSL GPIO 6
#define rtc_INT_PIN          2
#define waterSensor_PIN      3   

#define SENSOR_INT1 1
#define TIME_INT0   0

#define seconds 2
#define minutes 1
#define hours   0

/******Expansion board I2C address********/

#define singleChannelRS485_Add    0x50
#define disconnectorAdrr          0x21
#define resetBoardAdrr            0x24

/*****************************************/

float temp_1_Reading;
float temp_2_Reading;
float temp_3_Reading;
float temp_4_Reading;

uint8_t tempSensor_One_PIN    = 3;
uint8_t tempSensor_Two_PIN    = 7;
uint8_t tempSensor_Three_PIN  = A2;
uint8_t tempSensor_Four_PIN   = 5;
uint8_t batteryDisable_PIN    = 4;

#define sfx_Reset         digitalWrite(sigfoxReset_PIN,LOW)
#define wakeup_Pin_set    digitalWrite(sigfoxWake_PIN, LOW)
#define sfxReset_clr      digitalWrite(sigfoxReset_PIN,HIGH);
#define wakeup_Pin_clr    digitalWrite(sigfoxWake_PIN,HIGH);
#define battDisable       digitalWrite(batteryDisable_PIN,HIGH)

bool tempError = false;
uint8_t attempt = 0;

uint8_t  dataStoreCount;
uint8_t previousDay;
char I2C_address[3];
bool i2cScanComplete;

bool alarmTriggered = false;
bool pulseInterruptFlag = false;
bool sfx_SleeepStatus;
bool sfx_Init_Complete;
bool dailyPacket;
bool firstPulse;
bool pulseInterruptFlagp;
bool dataStoreMax = false;
bool gasStoreEn = false;
bool dataStoreEn = true;
uint8_t sfxRstcounter;
uint8_t packet[12];
uint8_t byteIndex;     //packet byte index position
uint16_t IAQ_level;
uint16_t Humidity_level, Temp_level;

uint32_t cumulativeFlow;
uint32_t flowRate;
//uint16_t minDailyFlow;

struct {
  uint16_t flowRate;
  uint8_t IAQ;
  uint8_t Humidity;
  uint8_t Temp;
  uint8_t DS18x_Temp_sen1;
  uint8_t DS18x_Temp_sen2;
  uint8_t DS18x_Temp_sen3;
  uint8_t DS18x_Temp_sen4;
  //  uint16_t tempHeader         = 2020;       //0x07E4    //Temperature DS18B20
  //  uint16_t airQualityHeader   = 3015;       //0x0BC7    //air quality
  //  uint16_t waterHeader        = 1030;       //0x0406    //water
  //  uint16_t electHeader        = 4030;       //0xBE0F
  //  uint16_t dailyPacket_Header = 1300;
  //  uint16_t minDailyFlow       = 15000;      //30 min maximun flow
} dataStrore[2];

void RTC_Init(void) {

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  //    rtc.setTime(16, 48, 50);            //hour, minute, Second (HH:MM:SS)
  //    rtc.setDate(9, Tue, Oct, 20);       //Tuesday 9th/10/2020
  rtc.DateTime(F(__DATE__), F(__TIME__)); // following line sets the RTC to the date & time this sketch was compiled
}

/*
  Header Table
  --------------------------------------------------------------------------
  haeder 1: 1030                                  |  Header 2:2020                       | Header 2: 3010
  1  device type (water)                         | *2  device type (temperature)        |*3 device type Air quality
  0  packet tarnsnission time unit in minute     | *0 tarnsnission time unit in minute  |*0 packet tarnsnission time unit in minute
  30 flow rate time 30 min                       | *20 temp reading every 20 min        |*10 flow rate time 10 min

*/

/*************downlink request data*****************/
//String sfx_downlinkRequest  = "AT$SF=FF02,1\r";
/*************downlink request data*****************/

char TXtimeUpdte[] =          "01,1\r";
char MeterCount[] =           "02,1\r";
char hardRest[]   =           "03,1\r";
char disconnectorRST[] =      "04,1\r";
char meterDisconnect[] =      "05,1\r";
char dwnlkTimeUpdate[] =      "06,1\r";
char packetRequest[]=         "08,1\r";

String downlink_msg = "";
char msgRead;
uint64_t RX_period;
uint16_t downlinkTimeout = 50000; // 50 seconds
uint8_t TXattempt = 0;
uint8_t maxAttempt = 2;

typedef enum {
  dataRequest,
  dataReceive,
  dataProcess,
  Standby
} rxState;

typedef struct {
  rxState state;
} Downlink;
Downlink downlink;
