/**
 * Homie Node for SknGarageDoor Node
 *
*/

#pragma once


extern volatile bool gbEnableDoorOperations;  // value managed by HomieEvent hook


#include <Automaton.h>
#include <Homie.hpp>
#include <Wire.h>
#include "SknLoxRanger.hpp"
#include "SknAtmDoor.hpp"
#include "SknAtmDigital.hpp"

class SknGarageDoor : public HomieNode {

public:
 
  SknGarageDoor(const char *id, const char *name, const char *cType, int rangerReadyPin, int relayPin);
  void setDoorState_cb(char *_state);
  void setDoorPosition_cb(unsigned int _position, unsigned int _requested);
  void updateDoorInfo();

protected:
  virtual void setup() override;
  virtual void onReadyToOperate() override;
  virtual bool handleInput(const HomieRange& range, const String& property, const String& value);
  virtual void loop() override;

private:
/*
 * Door travel: 86.5" or 2198 mm
 * Mount point: 13"   or  330 mm
 * maximum range:        2528 mm
*/
#define MM_MIN 330
#define MM_MAX 2528

  volatile bool vbOne=false;  // guard-flag to initialze machines only once; Homie setup/onReadyToOperate() seems to be called multiple times

  const char *cCaption = "• Garage Door Automaton Module:";
  const char *cIndent = " ✖  ";
  const char *cSknDoorID = "State";           // Door Positon Label; UP, DOWN, STOPPED,...
  const char *cSknPosID = "Position";         // Range 0 to 100; current position of door
  const char *cSknRestartID = "Reboot";       // service commander to force reboot of node
  
  const char *cDoorState = "Down";            // current door state/label
  int iDoorPosition = 100;                    // current door position; initially assumes door is down 
  int dataReadyPin = 13;                      // pin that controls relay; HIGH is active

  void enableAutomatons();                    // Initiales machines and inter-machine communications
  void printCaption();                        // utility

  SknLoxRanger  ranger;  // door position vl53l1x measurement
  SknAtmDoor    door;      // main door logic and relay
  SknAtmDigital irq;      // Ranger Position Automaton object
};
