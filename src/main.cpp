/**
  * Main.cpp
  *
 OpenHab Item
  Rollershutter	Roller shutter Item, typically used for blinds	
    Up
    Down 
    Stop
    Move 
    Percent

  This method provides a list of all data types that can be used to update the item state

Imagine e.g. a dimmer device: It's status could be 0%, 10%, 50%, 100%, but also OFF or ON and 
maybe UNDEFINED. So the accepted data types would be in this case 
  PercentType 0 ... 100
  OnOffType   ON ... OFF
  UnDefType   UNDEFINED

The order of data types denotes the order of preference. So in case a state needs to be converted 
in order to be accepted, it will be attempted to convert it to a type from top to bottom. Therefore the 
type with the least information loss should be on top of the list - in the example above the PercentType 
carries more information than the OnOffType, hence it is listed first.

Rollershutter	PercentType	 0...100
              UpDownType - UP if the shutter level indicated by the percent type equals 0, 
                           DOWN if it equals 100,
                           UnDefType.UNDEF for any other value


Configuration Settings section of data/Hhomie/config.json
  "settings": {
      "relayHoldTimeMS": 400,
      "positionIntervalSec": 60,
      "duration": 45,
      "rangerClosedMM": 2000,
      "rangerOpenMM": 200 }                           

*/

#include <Homie.hpp>
#include <Wire.h>
#include "SknGarageDoor.hpp"
#include "SknAtmDoor.hpp"
#include "SknAtmDigital.hpp"

extern "C"
{
#include <user_interface.h>
}

#define SKN_MOD_NAME "Garage Door Automation"
#define SKN_MOD_VERSION "0.0.1"
#define SKN_MOD_BRAND "SknSensors"

#define SKN_TITLE "Garage Door"
#define SKN_TYPE "Rollershutter"
#define SKN_ID "SknGarageDoor"

// Pins
#define SDA 5
#define SCL 4
#define LOX_GPIO  13
#define RELAY_GPIO     12

#ifndef LED_BUILTIN
  #define LED_BUILTIN 4
#endif


/* Automaton Nodes 
  Door Command     Door Event              StateChange           Action
  ---------------------------------------------------------------------------------------------------------------
  Homie-cmd_pos    trigger(EVT_[UP,DOWN])  MOVING_[UP,DOWN]->UP  ENT_MOVING_[UP,DOWN]->move[Up,Dn](relay,ranger)
  Homie-cmd_up     trigger(EVT_UP)         MOVING_UP->UP         ENT_MOVING_UP->moveUp(relay,ranger)
  Homie-cmd_down   trigger(EVT_DOWN)       MOVING_DOWN->DOWN     ENT_MOVING_DOWN->moveDn(relay,ranger)
  Homie-cmd_stop   trigger(EVT_STOP)       <any>->STOPPED        ENT_STOPPED->moveStp(relay,ranger)

  Homie                                <--:OnChange_cb(state)
  Homie                                <--:OnPos_cb(position)
                    EVT_POS_REACHED        UP,DOWN               <--LP_POS->moveHalt(ranger)
  Door.setDoorPosition()                                         moveChgDir(relay)                  
  Irq.onChange_cb(dataReady)                                     translate mm to range and send to Doors

*/
SknAtmDigital irq;                   // handles data ready interrupt for ranger
SknLoxRanger ranger;                 // measures distance of door
SknAtmDoor door(RELAY_GPIO, ranger); // controls door relay and startng stopping of ranger

/* Homie Nodes 
*/
SknGarageDoor doorNode(SKN_ID, SKN_TITLE, SKN_TYPE, LOX_GPIO, irq, ranger, door); // communication interface

/*
 * Callback for Homie Broadcasts
*/
bool broadcastHandler(const String &level, const String &value)
{
  Homie.getLogger() << "Received broadcast level " << level << ": " << value << endl;
  return true;
}

/*
 * Arduino Setup: Initialze Homie
*/
void setup()
{
  delay(200);
  Serial.begin(115200);
  if (!Serial)
  {
    Homie.disableLogging();
  }

  Wire.begin(SDA, SCL);

  Homie_setFirmware(SKN_MOD_NAME, SKN_MOD_VERSION);
  Homie_setBrand(SKN_MOD_BRAND);

  Homie.setBroadcastHandler(broadcastHandler)
      .setLedPin(LED_BUILTIN, LOW)
      .disableResetTrigger();

  Homie.setup();
}

/*
 * Arduino Loop: Cycles Homie Nodes
*/
void loop()
{
  Homie.loop();
}
