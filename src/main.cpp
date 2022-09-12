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

extern "C"
{
#include <user_interface.h>
}

#define SDA 5
#define SCL 4
#define DEFAULT_HOLD_MS 400
#define LOX_RUNTIME_SECONDS 30

#define SKN_MOD_NAME "Garage Door Automation"
#define SKN_MOD_VERSION "0.0.1"
#define SKN_MOD_BRAND "SknSensors"

#define SKN_TITLE "Garage Door"
#define SKN_TYPE "Rollershutter"
#define SKN_ID "SknGarageDoor"

// Pins
#define LOX_PIN_SDA   SDA
#define LOX_PIN_SCL   SCL
#define LOX_PIN_GPIO  13
#define RELAY_PIN    16

#ifndef LED_BUILTIN
  #define LED_BUILTIN 4
#endif

/* Automaton Nodes 
*/
Atm_digital irq;                    // handles data ready interrupt for ranger
SknLoxRanger ranger;                // measures distance of door
SknAtmDoor door(RELAY_PIN, ranger); // controls door relay and startng stopping of ranger

/* Homie Nodes 
*/
SknGarageDoor doorNode(SKN_ID, SKN_TITLE, SKN_TYPE, LOX_PIN_GPIO, irq, ranger, door); // communication interface

/*
 * Callback for Ranger positioning
*/
void readDoorPositionCallback(int idx, int v, int up ) {
  long posValue = (long)ranger.readValues(false);
  long translatedValue = map(posValue, 10, 1960, 0, 100);
  posValue = constrain(translatedValue, 0, 100);

  door.setDoorPosition( posValue );
  doorNode.setDoorPosition( posValue );

  Serial.printf("Door position = %ld, idx=%d, v=%d, up=%d\n", posValue, idx,v,up);
}  
/*
 * Callback for Door State
*/
void readDoorStateCallback(int idx, int v, int up ) {
  const char *_state = door.mapstate(v);
  doorNode.setDoorState( (char *)_state );

  Serial.printf("Door State = %s\n", _state);
}  

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

  Wire.begin(LOX_PIN_SDA, LOX_PIN_SCL);

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
