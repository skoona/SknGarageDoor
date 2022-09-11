/**
 * Homie Node for SknGarageDoor Node
 *
*/

#pragma once

#include <Automaton.h>
#include <Homie.hpp>
#include <Wire.h>
#include "SknLoxRanger.hpp"
#include "SknAtmDoor.hpp"

extern void readDoorPositionCallback(int idx, int v, int up );

class SknGarageDoor : public HomieNode {

public:
 
  SknGarageDoor(const char *id, const char *name, const char *cType, int rangerReadyPin, Atm_digital& irqObj, SknLoxRanger& rangerObj, SknAtmDoor& doorObj);
  void setDoorState(char *_state);
  void setDoorPosition(unsigned int _position);

protected:
  virtual void setup() override;
  virtual void onReadyToOperate() override;
  virtual bool handleInput(const HomieRange& range, const String& property, const String& value);
  virtual void loop() override;

private:

  volatile bool vbOne=true;

  const char *cCaption = "• Garage Door Automaton Module:";
  const char *cIndent = " ✖  ";
  const char *cSknDoorID = "State";
  const char *cSknPosID = "Position";
  
  char *cSknDoorState = "Down";
  int iDoorPosition = 100;
  int dataReadyPin = 13;

  void printCaption();

  SknLoxRanger& ranger;  // door position vl53l1x measurement
  SknAtmDoor& door;      // main door logic and relay
  Atm_digital& irq;      // Ranger Position Automaton object
};
