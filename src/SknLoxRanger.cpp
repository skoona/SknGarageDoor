/**
 * Automaton Class for VL53L1x.
 *
 */
#include "SknLoxRanger.hpp"

SknLoxRanger::SknLoxRanger( ) {}

/**
 * VL53L1x Device Init
 */
SknLoxRanger& SknLoxRanger::begin(int gpioPin, unsigned int interMeasurementDurationMS ) {
  unsigned long int time_now = millis();

  pinGPIO = gpioPin;
  uiIinterMeasurementDuration=interMeasurementDurationMS;

  lox.setTimeout(500);
 
  if (!lox.init())
  {
    // delay(1000); // Delay is not working in this class ???
    time_now = millis();
    while (millis() < time_now + 1000){}

    time_now = millis();
    while (!lox.init())
    {
      Serial.printf(" ✖  Failed to detect and initialize sensor!");
      time_now = millis();
      while (millis() < time_now + 1000){}
    }
  }

  if (lox.setDistanceMode(VL53L1X::Medium))
  {
    Serial.printf("〽 Medium distance mode accepted.");
  }

  if (lox.setMeasurementTimingBudget(200000))
  {
    Serial.printf("〽 200ms timing budget accepted.");
  }

  return(*this);
}

/**
 *
 */
void SknLoxRanger::rangerStart(uint32_t duration) {
  lox.startContinuous(duration);
}

/**
 *
  if (!digitalRead(pinGPIO))
  - active low
 */
unsigned int SknLoxRanger::rangerReadValues()
{
  const int capacity = (MAX_SAMPLES);

  unsigned int value = (unsigned int)lox.read(false);
  if (value == 0) {
    return uiDistanceValue;
  }

  for (int idx = 0; idx < capacity; idx++) {
    distances[idx] = distances[idx+1]; // move all down
  }

  if (lox.ranging_data.range_status == 0) {
    distances[capacity] = lox.ranging_data.range_mm;
    uiDistanceValue = lox.ranging_data.range_mm;
  } else {
    distances[capacity] = uiDistanceValue;
  }

  Serial.printf("〽 range: %ul mm,\tstatus: %s\traw: %ul\tsignal: %3.1f MCPS\tambient: %3.1f MCPS",
                    lox.ranging_data.range_mm,
                    lox.rangeStatusToString(lox.ranging_data.range_status),
                    lox.ranging_data.range_status,
                    lox.ranging_data.peak_signal_count_rate_MCPS,
                    lox.ranging_data.ambient_count_rate_MCPS);
  
  
  return uiDistanceValue;
}

/**
 *
 */
void SknLoxRanger::rangerStop() {
  lox.stopContinuous();  
}
