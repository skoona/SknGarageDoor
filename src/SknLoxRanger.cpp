/**
 * Automaton Class for VL53L1x.
 *
 */
#include "SknLoxRanger.hpp"

SknLoxRanger::SknLoxRanger( ) {
  for (int idx = 0; idx < (MAX_SAMPLES); idx++) {
      distances[idx] = 0;
  }
}

/**
 * VL53L1x Device Init
 */
SknLoxRanger& SknLoxRanger::begin( unsigned int interMeasurementDurationMS=1000 ) {
  unsigned long int time_now = millis();

  uiInterMeasurementDuration=interMeasurementDurationMS;

  lox.setTimeout(512);

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
    Serial.printf(" ✖  Exited initialize sensor!\n");
  }

  lox.setTimeout(512);

  if (lox.setDistanceMode(VL53L1X::Medium))
  {
    Serial.printf("〽 Medium distance mode accepted.\n");
  }

  if (lox.setMeasurementTimingBudget(300000))
  {
    Serial.printf("〽 300ms timing budget accepted.\n");
  }
  
  Serial.printf(" ✖  SknLoxRanger initialization Complete.\n");
  
  stop();

  return(*this);
}

/**
 *  // 250ms read + 250ms wait = cycle time
 */
SknLoxRanger&  SknLoxRanger::start() {
  lox.startContinuous(uiInterMeasurementDuration);
  return *this;
}

/**
 *
  if (!digitalRead(pinGPIO))
  - active low
 */
unsigned int SknLoxRanger::readValues(bool wait=false)
{
  const int capacity = (MAX_SAMPLES);
  unsigned long sum = 0;
  unsigned long avg = 0;

  unsigned int value = (unsigned int)lox.readRangeContinuousMillimeters(wait); // readRangeContinuousMillimeters(true)
  if (value <= 0) {
    return uiDistanceValue;
  }

  for (int idx = 0; idx < capacity; idx++) {
    if(distances[idx+1]>=5000) {
      distances[idx] = value;
    }else {
      distances[idx] = distances[idx+1]; // move all down
    }
    sum += distances[idx];
  }

  if (lox.ranging_data.range_status == 0) {
    distances[capacity] = value;
    sum += distances[capacity];
    avg = (sum / (capacity +1));
    uiDistanceValue = (unsigned int) avg;
    // uiDistanceValue = value;
  } else {
    distances[capacity] = uiDistanceValue;
  }

  Serial.printf("〽 range: %u mm avgerage: %lu mm,\tstatus: %s\traw: %u\tsignal: %3.1f MCPS\tambient: %3.1f MCPS\n",
                    value,
                    avg,
                    lox.rangeStatusToString(lox.ranging_data.range_status),
                    lox.ranging_data.range_status,
                    lox.ranging_data.peak_signal_count_rate_MCPS,
                    lox.ranging_data.ambient_count_rate_MCPS);
  
  
  return distances[capacity];
}

/**
 *
 */
void SknLoxRanger::stop() {
  lox.stopContinuous();  
}
