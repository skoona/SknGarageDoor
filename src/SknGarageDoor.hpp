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
#include "SknAtmDigital.hpp"

class SknGarageDoor : public HomieNode {

public:
 
  SknGarageDoor(const char *id, const char *name, const char *cType, int rangerReadyPin, SknAtmDigital& irqObj, SknLoxRanger& rangerObj, SknAtmDoor& doorObj);
  void setDoorState(char *_state);
  void setDoorPosition(unsigned int _position);

protected:
  virtual void setup() override;
  virtual void onReadyToOperate() override;
  virtual bool handleInput(const HomieRange& range, const String& property, const String& value);
  virtual void loop() override;

private:

  volatile bool vbOne=false;

  const char *cCaption = "• Garage Door Automaton Module:";
  const char *cIndent = " ✖  ";
  const char *cSknDoorID = "State";
  const char *cSknPosID = "Position";
  
  const char *cSknDoorState = "Down";
  int iDoorPosition = 100;
  int dataReadyPin = 13;

  void enableAutomatons();
  void printCaption();

  SknLoxRanger& ranger;  // door position vl53l1x measurement
  SknAtmDoor& door;      // main door logic and relay
  SknAtmDigital& irq;      // Ranger Position Automaton object
};
