/**
 * Automaton Class for VL53L1x.
 * - Atm_digital pulse;
 */

#pragma once

// #include <Automaton.h>
#include <VL53L1X.h>

extern "C"
{
#include <user_interface.h>
}

class SknLoxRanger {

public:
  SknLoxRanger();

  SknLoxRanger& begin(int gpioPin, unsigned int interMeasurementDurationMS );
  void rangerStart(uint32_t duration);
  unsigned int rangerReadValues();
  void rangerStop();

protected:
  int pinGPIO;
  unsigned int uiDistanceValue = 0;
  uint32_t uiIinterMeasurementDuration;
  
private :   
  #define MAX_SAMPLES 5
  unsigned int distances[MAX_SAMPLES + 2];

  VL53L1X lox; 
};
