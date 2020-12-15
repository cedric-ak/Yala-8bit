/**********************************************************
**Name:     CPU WakeUp
**Function: CPU wake up routine excute water process
**Input:    none
**Output:   none
**note:     this function is define multiple times for different applicatons
**********************************************************/
#ifdef WATER_APPLICATION
void CPU_WakeUp(void) {

  if (pulseInterruptFlag == true) {       //if CPU awaken by meter pulse
    if (firstPulse == true) {             //make sure first pulse is ignored then start flow count
      cumulativeFlow++;
      flowRate++;
      Serial.print("Total pulse count = ");
      Serial.println(cumulativeFlow);
    } else {                      //ignore first pulse
      firstPulse = true;
    }
    pulseInterruptFlag = false;   //clear pulse flag status
    APP_STATE()  = CPU_SLEEP;     //go back to sleep after count
  }

  else if (alarmTriggered == true) {     //rtc interrupt
    rtc.INTF_CLR(CTAF);                  //Clear rtc Timer A count down interrupt flag
    APP_STATE() = CPU_DATA_STORE;
  }
}
#endif
