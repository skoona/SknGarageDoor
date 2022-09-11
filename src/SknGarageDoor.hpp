/**
 * Homie Node for SknGarageDoor Node
 *
*/

#pragma once

#include <Automaton.h>
#include <Homie.hpp>
#include <Wire.h>
#include "SknLoxRanger.hpp"

extern void readDoorPositionCallback(int idx, int v, int up );

class SknGarageDoor : public HomieNode {

public:
 
  SknGarageDoor(const char *id, const char *name, const char *cType, SknLoxRanger& rangerObj);
  void setDoorPosition(unsigned int _position);

protected:
  virtual void setup() override;
  virtual void onReadyToOperate() override;
  virtual bool handleInput(const HomieRange& range, const String& property, const String& value);
  virtual void loop() override;

private:

  volatile bool vbOne=true;

  const char *cCaption = "• Garage Door Automation Module:";
  const char *cIndent = " ✖  ";
  const char *cSknDoorID = "State";
  const char *cSknPosID = "Position";
  
  unsigned int uiDistanceValue = 0;

  void printCaption();

  SknLoxRanger& ranger;
  Atm_digital irq;   // placeholder Automaton object
};
