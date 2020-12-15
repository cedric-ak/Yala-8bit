
#include <SparkFunBQ27441.h>
extern LED led;
#define BATTERY_CAPACITY  2000

void setupBQ27441(void)
{
  // Use lipo.begin() to initialize the BQ27621 and confirm that it's
  // connected and communicating.
  if (!lipo.begin()) // begin() will return true if communication is successful
  {
    // If communication fails, print an error message and loop forever.
    Serial.println("no fuel Guage!");
    //    while (1) {
          led.blinkTime(RED,3,500);
    //    }
  }

  // Uset lipo.setCapacity(BATTERY_CAPACITY) to set the design capacity
  // of your battery.
  lipo.setCapacity(BATTERY_CAPACITY);
}

/**********************************************************
**Name:     BatteryHealth()
**Function: CHecks battery heath status
**Input:    none
**Output:   battery heatlth readings
**note:
**********************************************************/
void BatteryHealth()
{
  // Read battery stats from the BQ27621

  //  unsigned int soc, volts, fullCapacity, capacity;
  //  int current, power, health;
  //
  //  soc = lipo.soc();  // Read state-of-charge (%)
  //  volts = lipo.voltage(); // Read battery voltage (mV)
  //  current = lipo.current(AVG); // Read average current (mA)
  //  fullCapacity = lipo.capacity(FULL); // Read full capacity (mAh)
  //  capacity = lipo.capacity(REMAIN); // Read remaining capacity (mAh)
  //  power = lipo.power(); // Read average power draw (mW)
  //  health = lipo.soh(); // Read state-of-health (%)
  //
  //  // Now print out those values:
  //  String toPrint = String(soc) + "% | ";
  //  toPrint += String(volts) + " mV | ";
  //  toPrint += String(current) + " mA | ";
  //  toPrint += String(capacity) + " / ";
  //  toPrint += String(fullCapacity) + " mAh | ";
  //  toPrint += String(power) + " mW | ";
  //  toPrint += String(health) + "%";

  //  Serial.print("Battery Health:");
  //  Serial.print("\t");
  //  Serial.println(toPrint);
}

uint8_t BatteryState(void) {
  Serial.println("Battery:" + String(lipo.soc()) + "%");
  return lipo.soc();
}

void battDisabled(void) {
  battDisable;
}

void batteryToggle(void) {
  for (int cnt = 0; cnt < 2; cnt++) {
    digitalWrite(batteryDisable_PIN, !digitalRead(batteryDisable_PIN));
    delay(1000);
  }
}

void fuelGuageHibernate(void) {
  lipo.hibernate();
}
