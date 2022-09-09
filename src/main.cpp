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

*/

#include <Homie.hpp>
#include <Wire.h>
#include "SknGarageDoor.hpp"

extern "C"
{
#include <user_interface.h>
}

#define SDA 14
#define SCL 12
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
#define RELAY_PIN    5

// #ifndef LED_BUILTIN
#define LED_BUILTIN 2
// #endif

HomieSetting<long> cfgRelayHoldMS("relayHoldTimeMS", "Relay hold time in milliseconds.");
HomieSetting<long> cfgIntervalSec("positionIntervalSec", "Seconds between ranging to verify door position.");
HomieSetting<long> cfgDuration("duration", "Seconds to measure distance after triggered.");
HomieSetting<long> cfgOpenMM("rangerOpenMM", "fully open threshold in millimeters.");
HomieSetting<long> cfgClosedMM("rangerClosedMM", "fully closed threshold in millimeters.");

SknGarageDoor doorNode(SKN_ID, SKN_TITLE, SKN_TYPE);

bool broadcastHandler(const String &level, const String &value)
{
  Homie.getLogger() << "Received broadcast level " << level << ": " << value << endl;
  return true;
}

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

  cfgRelayHoldMS
      .setDefaultValue(DEFAULT_HOLD_MS)
      .setValidator([](long candidate)
                    { return candidate > 200 && candidate < 1000; });
  cfgIntervalSec
      .setDefaultValue(300)
      .setValidator([](long candidate)
                    { return candidate > 59 && candidate < 3601; });
  cfgDuration
      .setDefaultValue(20)
      .setValidator([](long candidate)
                    { return candidate > 0 && candidate < 181; });

  cfgOpenMM
      .setDefaultValue(200)
      .setValidator([](long candidate)
                    { return candidate > 10 && candidate < 400; });
  cfgClosedMM
      .setDefaultValue(2000)
      .setValidator([](long candidate)
                    { return candidate > 1000 && candidate < 3200; });

  // relay.setHoldTimeInMilliseconds(cfgRelayHoldMS.get());
  // ranger.setRunDuration(cfgDuration.get());
  // ranger.setOpenThresholdMM(cfgOpenMM.get());
  // ranger.setClosedThresholdMM(cfgClosedMM.get());
  // doorNode.setIntervalInSeconds(cfgIntervalSec.get());

  Homie.setBroadcastHandler(broadcastHandler)
      .setLedPin(LED_BUILTIN, LOW)
      .disableResetTrigger();

  Homie.setup();
}

void loop()
{
  Homie.loop();
}
