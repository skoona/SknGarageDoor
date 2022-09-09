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
  SknLoxRanger(const int gpioPin);

  bool isReady();
  bool isOpen();
  bool isClosed();
  void operate();
  void setRunDuration(const int seconds);
  void setOpenThresholdMM(const int mm);
  void setClosedThresholdMM(const int mm);

protected:

  virtual void loop() override;
  virtual void onReadyToOperate() override;
  unsigned long setDuration(unsigned long duration) ;
  
private : 

  int _pinGPIO;
  unsigned long _ulCycleTime;
  unsigned int uiDistanceValue = 0;
  #define MAX_SAMPLES 5
  unsigned int distances[MAX_SAMPLES + 2];
  long thresholdOpen;
  long thresholdClosed;
  
  char cDirection[32]; // CLOSED, OPEN, ClOSING, OPENING

  const char *cCaption = "• VL53L1x Ranging Module:";
  const char *cIndent = " ✖  ";

  const char *cRangeID = "range";
  const char *cRangeFormat = "%04d";
  const char *cStatusID = "status";
  const char *cStatusFormat = "%s";
  const char *cSignalID = "signal";
  const char *cSignalFormat = "%04.2f";
  const char *cAmbientID = "ambient";
  const char *cAmbientFormat = "%03.2f";
  const char *cOperateID = "ranging";
  const char *cOperateFormat = "ON,OFF";
  const char *cDirectionID = "direction";
  const char *cDirectionFormat = "CLOSING,OPENING,IDLE";


  unsigned long ulTimebase = 0,             // current ms count
                ulLastTimebase = 0,         // ms from last operation
                ulCycleDuration = 30000,    // time window to operate
                ulRangingDuration = 250,    // distance read time
                ulElapsedTime = 0;          // elapsed from timebase

  volatile bool vbRangeDuration = false,    // time to read lox
                vbLastRangeCycle = true,    // lox off signal
                vbRunCycle = false,         // run top level for ulCycleDuration
                vbEnabled = false;          // operating trigger

  void startRanging(uint32_t duration);
  bool collectRangeValues();
  void stopRanging();

  void printCaption();
  unsigned int handleLoxRead();
  void setDirectionStatus(const int value);

  VL53L1X lox; 

  enum DoorPosition {
    IDLE,
    OPEN,
    OPENING,
    CLOSED,
    CLOSING
  };

};
