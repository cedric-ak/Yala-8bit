#ifdef AIR_QUALITY

#define SEALEVELPRESSURE_HPA (1013.25)

float hum_weighting = 0.25; // so hum effect is 25% of the total air quality score
float gas_weighting = 0.75; // so gas effect is 75% of the total air quality score

int   humidity_score, gas_score;
float gas_reference = 2500;
float hum_reference = 40;
int   getgasreference_count = 0;
int   gas_lower_limit = 10000;  // Bad air quality limit
int   gas_upper_limit = 300000; // Good air quality limit

/**********************************************************
**Name:     CPU WakeUp
**Function: cup wake up routine excute air quality(humidity, temperature, eCO2 level
**Input:    none
**Output:   none
**note:     this function is define multiple times for different applicatons
**********************************************************/

void CPU_WakeUp(void) {

  IAQ_level = 0;
  Serial.println("Sensor Readings:");
  Serial.println("  Temperature = " + String(bme.readTemperature(), 2)     + "°C");
  Serial.println("     Pressure = " + String(bme.readPressure() / 100.0F) + " hPa");
  Serial.println("     Humidity = " + String(bme.readHumidity(), 1)        + "%");
  Serial.println("          Gas = " + String(gas_reference)               + " ohms\n");

  humidity_score = GetHumidityScore();
  gas_score      = GetGasScore();

  //Combine results for the final IAQ index value (0-100% where 100% is good quality air)
  float air_quality_score = humidity_score + gas_score;
  Serial.println(" comprised of " + String(humidity_score) + "% Humidity and " + String(gas_score) + "% Gas");
  if ((getgasreference_count++) % 5 == 0) GetGasReference();
  Serial.println(CalculateIAQ(air_quality_score));
  //  Serial.println("--------------------------------------------------------------");
  delay(2000);

  Temp_level = bme.readTemperature();
  Humidity_level = bme.readHumidity();
  APP_STATE()  = CPU_DATA_STORE;

}

void GetGasReference() {
  // Now run the sensor for a burn-in period, then use combination of relative humidity and gas resistance to estimate indoor air quality as a percentage.
  //Serial.println("Getting a new gas reference value");
  int readings = 10;
  for (int i = 1; i <= readings; i++) { // read gas for 10 x 0.150mS = 1.5secs
    gas_reference += bme.readGas();
    //    delay(250);
  }
  gas_reference = gas_reference / readings;
  //Serial.println("Gas Reference = "+String(gas_reference,3));
}


String CalculateIAQ(int score) {
  String IAQ_text = "air quality is ";
  score = (100 - score) * 5;
  IAQ_level = score;
  if      (score >= 301)                  IAQ_text += "Hazardous";
  else if (score >= 201 && score <= 300 ) IAQ_text += "Very Unhealthy";
  else if (score >= 176 && score <= 200 ) IAQ_text += "Unhealthy";
  else if (score >= 151 && score <= 175 ) IAQ_text += "Unhealthy for Sensitive Groups";
  else if (score >=  51 && score <= 150 ) IAQ_text += "Moderate";
  else if (score >=  00 && score <=  50 ) IAQ_text += "Good";
  Serial.print("IAQ Score = " + String(score) + ", ");
  return IAQ_text;
}

int GetHumidityScore() {  //Calculate humidity contribution to IAQ index
  float current_humidity = bme.readHumidity();
  if (current_humidity >= 38 && current_humidity <= 42) // Humidity +/-5% around optimum
    humidity_score = 0.25 * 100;
  else
  { // Humidity is sub-optimal
    if (current_humidity < 38)
      humidity_score = 0.25 / hum_reference * current_humidity * 100;
    else
    {
      humidity_score = ((-0.25 / (100 - hum_reference) * current_humidity) + 0.416666) * 100;
    }
  }
  return humidity_score;
}

int GetGasScore() {
  //Calculate gas contribution to IAQ index
  gas_score = (0.75 / (gas_upper_limit - gas_lower_limit) * gas_reference - (gas_lower_limit * (0.75 / (gas_upper_limit - gas_lower_limit)))) * 100.00;
  if (gas_score > 75) gas_score = 75; // Sometimes gas readings can go outside of expected scale maximum
  if (gas_score <  0) gas_score = 0;  // Sometimes gas readings can go outside of expected scale minimum
  return gas_score;
}

void airQuality(char dataIndex) {
  dataStrore[dataIndex].IAQ      = IAQ_level / 2;     //multiply by two on back end
  dataStrore[dataIndex].Humidity = Humidity_level;
  dataStrore[dataIndex].Temp     = Temp_level;
}
#endif
