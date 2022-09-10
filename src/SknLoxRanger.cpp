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

  if (!lox.init())
  {
    // delay(1000); // Delay is not working in this class ???
    time_now = millis();
    while (millis() < time_now + 1000){}

    time_now = millis();
    while (!lox.init())
    {
      Serial.printf(" ✖  Failed to detect and initialize sensor!\n");
      time_now = millis();
      while (millis() < time_now + 1000){}
    }
  }

  lox.setTimeout(500);
 
  if (lox.setDistanceMode(VL53L1X::Medium))
  {
    Serial.printf("〽 Medium distance mode accepted.\n");
  }

  if (lox.setMeasurementTimingBudget(250000))
  {
    Serial.printf("〽 250ms timing budget accepted.\n");
  }
  
  Serial.printf(" ✖  SknLoxRanger initialization Complete.\n");
  return(*this);
}

/**
 *  // 250ms read + 250ms wait = cycle time
 */
SknLoxRanger&  SknLoxRanger::rangerStart() {
  lox.startContinuous(uiIinterMeasurementDuration);
  return *this;
}

/**
 *
  if (!digitalRead(pinGPIO))
  - active low
 */
unsigned int SknLoxRanger::rangerReadValues(bool wait=false)
{
  const int capacity = (MAX_SAMPLES);

  unsigned int value = (unsigned int)lox.readRangeContinuousMillimeters(wait); // readRangeContinuousMillimeters(true)
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

  Serial.printf("〽 range: %u mm,\tstatus: %s\traw: %u\tsignal: %3.1f MCPS\tambient: %3.1f MCPS\n",
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
