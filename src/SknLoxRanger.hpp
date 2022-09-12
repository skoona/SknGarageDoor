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

  SknLoxRanger& begin( unsigned int interMeasurementDurationMS );
  SknLoxRanger&  start();
  unsigned int readValues(bool wait);
  void stop();

protected:
  unsigned int uiDistanceValue = 0;
  unsigned int uiIinterMeasurementDuration;
  
private :   
  #define MAX_SAMPLES 5
  unsigned int distances[MAX_SAMPLES + 2];

  VL53L1X lox; 
};
