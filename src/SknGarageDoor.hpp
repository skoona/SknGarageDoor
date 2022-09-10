/**
 * Homie Node for SknGarageDoor Node
 *
*/

#pragma once

#include <Automaton.h>
#include <Homie.hpp>
#include <Wire.h>
#include "SknLoxRanger.hpp"


class SknGarageDoor : public HomieNode {

public:
  SknGarageDoor(const char *id, const char *name, const char *cType);

  void setIntervalInSeconds(const long seconds);

protected:
  virtual void setup() override;
  virtual void onReadyToOperate() override;
  virtual bool handleInput(const HomieRange& range, const String& property, const String& value);
  virtual void loop() override;

private:

  volatile bool vbOne=true;

  Atm_digital irq;   // placeholder Automaton object

  const char *cCaption = "• Garage Door Automation Module:";
  const char *cIndent = " ✖  ";
  const char *cSknGarageDoorID = "State";
  const char *cSknGarageDoorPositionID = "Position";

  void printCaption();

  SknLoxRanger ranger;
};
