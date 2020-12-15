#ifdef ENERGY_MONITOR_APPLICATION
enum EnergyMonitorStatus : uint8_t { EMS_IDLE = 0, EMS_CALC_POWER = 1, EMS_WAIT_POWERON = 2, EMS_CHECK_SEND_SIGFOX = 3, EMS_SEND_SIGFOX = 4};
EnergyMonitorStatus energyMonitorStatus;
void transitionState(EnergyMonitorStatus newStatus); 
uint8_t oldstatus = 255;



#define Sprintln(a)                               (Serial.println(a)) //This definition allows for serial printing to be turned off in bulk by removing the definition
#define Sprint(b)                                 (Serial.print(b))   //This definition allows for serial printing to be turned off in bulk by removing the definition
#define S_println(a)                              (Serial.println(a)) //This definition allows for serial printing to be turned off in bulk by removing the definition
#define S_print(b)                                (Serial.print(b))   //This definition allows for serial printing to be turned off in bulk by removing the definition

extern LED led;

long powerOnTime = 0;
long waitPowerOn = 60000;
long startWaitPowerOn = 0;
bool rtcFiredPowerOn = false;

long lastLedStatus = 0;
long ledStatus = 20000;

void CPU_WakeUp(void) {
  //Serial.println("Wakeup:");
  //Serial.println(devcieWakeup);
  if( devcieWakeup == true) {
    //Serial.println("Wakeup = T");
    devcieWakeup = false;
    //led.blink(BLUE,300);
    //delay(1000);
    transitionState(EMS_IDLE);
    APP_STATE() = SFX_CHECK_SEND;
  }
  switch (energyMonitorStatus) {
    case EMS_IDLE:
      attachInterrupt (TIME_INT0, rtcMatch, FALLING); 
      checkprintState();
      transitionState(EMS_CALC_POWER);
    break;
    case EMS_CALC_POWER:
      checkprintState();
      checkPower();
    break;
    case EMS_WAIT_POWERON:
      checkrtcFired();
      checkWaitPowerOn();
    break;
    case EMS_CHECK_SEND_SIGFOX:
    
    break;
    case EMS_SEND_SIGFOX:
      checkprintState();
    break;
  }
}
void checkrtcFired() {
  if (rtcFiredPowerOn == true) {
    rtcFiredPowerOn = false;
    Serial.println("RTC Fired");
    rtcWakeupCounter++;
    packetToTX = 0;
    APP_STATE() = SFX_CHECK_SEND;
  }
}
void rtcMatch() {
  rtcFiredPowerOn = true;
}
void checkWaitPowerOn() {
  //Serial.println("In here");
  if (millis() - startWaitPowerOn > waitPowerOn) {
    //Serial.println("Recalc");
    transitionState(EMS_CALC_POWER);
  }
  checkDeviceLedStatus();
}
void checkDeviceLedStatus() {
  if (millis() - lastLedStatus > ledStatus) {
    led.blinkTime(MAGENTA,3,300);   //blink red three times if no meter found
    lastLedStatus = millis();
  }
}
void checkPower() {
  
  powerState = analogRead(powerPin);
  //Serial.println(powerState);
  if (powerState > 100) {
    led.blink(RED,300);
    delay(1000);
    if (APP_STATE() == SFX_CHECK_SEND) {

    } else {
      APP_STATE() = SFX_SLEEP; 
    } 
  } else {
    startWaitPowerOn = millis();
    led.blink(BLUE,300);  
    powerOnTime += (waitPowerOn/60000);
    Serial.print("P_On:");
    Serial.println(powerOnTime);
    transitionState(EMS_WAIT_POWERON);
  }
}
void transitionState(EnergyMonitorStatus newStatus) {
  energyMonitorStatus  = newStatus;
}
void checkprintState() {

  if (oldstatus != energyMonitorStatus) {
    Sprint(F("S:"));
    Sprintln(energyMonitorStatus);
    oldstatus = energyMonitorStatus;
  }

}
#endif
