/**
  * Main.cpp
  *
 OpenHab Item
  Rollershutter	Roller shutter Item, typically used for blinds	
    Up
    Down 
    Stop    
    0:100 Percent

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
  "settings": { }                           

*
* Automaton Notes: https://github.com/tinkerspy/Automaton/wiki/
  
  Door Command     Door Event              StateChange           Action
  ---------------------------------------------------------------------------------------------------------------
  Homie-cmd_pos    trigger(EVT_POS)        MOVING_POS->STOPPED   ENT_POS->doorMove(relay,ranger)
  Homie-cmd_up     trigger(EVT_UP)         MOVING_UP->UP         ENT_MOVING_UP->doorMove(relay,ranger)
  Homie-cmd_down   trigger(EVT_DOWN)       MOVING_DOWN->DOWN     ENT_MOVING_DOWN->doorMove(relay,ranger)
  Homie-cmd_stop   trigger(EVT_STOP)       <any>->STOPPED        ENT_STOPPED->doorStop(relay,ranger)

  Homie                                <--:OnChange_cb(state)
  Homie                                <--:OnPos_cb(position)
                    EVT_POS_REACHED        UP,DOWN,STOPPED     <--LP_POS->doorHalt(ranger)
  Door.setDoorPosition_cb()                                         doorChangeDirection(relay)                  
  Irq.onChange_cb(dataReady)                                     translate mm to range and send to Doors

Note:
  1. If input asks to move state to current state, request will be ignored
  2. Ranger (VL53L1x) will cycle briefly on boot to fill its internal averaging buffer.
  3. 100 is considered DOWN, with 0 considered UP.

        [-- STATES --]   [------- Actions/External Outputs ----------] [-------------------------------------------- Event/External Inputs ---------------------------------------------] 
                                    ON_ENTER       ON_LOOP    ON_EXIT      EVT_DOWN   EVT_STOP       EVT_UP       EVT_POS  EVT_POS_REACHED  EVT_TIMER   EVT_LEARN_UP  EVT_LEARN_DOWN  ELSE 
             STOPPED            ENT_STOPPED,           -1,        -1,  MOVING_DOWN,        -1,   MOVING_UP,   MOVING_POS,              -1,        -1,     LEARN_UP,      LEARN_DOWN,   -1,
           MOVING_UP          ENT_MOVING_UP,       LP_POS,        -1,  MOVING_DOWN,   STOPPED,          -1,           -1,              UP,        -1,           -1,              -1,   -1,
                  UP                 ENT_UP,           -1,        -1,  MOVING_DOWN,   STOPPED,          -1,   MOVING_POS,              -1,        -1,     LEARN_UP,      LEARN_DOWN,   -1,
         MOVING_DOWN        ENT_MOVING_DOWN,       LP_POS,        -1,           -1,   STOPPED,   MOVING_UP,           -1,            DOWN,        -1,           -1,              -1,   -1,
                DOWN               ENT_DOWN,           -1,        -1,           -1,   STOPPED,   MOVING_UP,   MOVING_POS,              -1,        -1,     LEARN_UP,      LEARN_DOWN,   -1,
          MOVING_POS               ENT_POS,        LP_POS,        -1,           -1,   STOPPED,          -1,           -1,         STOPPED,        -1,           -1,              -1,   -1,
            LEARN_UP             ENT_LEARN,      LP_LEARN,        -1,           -1,   STOPPED,          -1,           -1,              -1,        STOPPED,      -1,              -1,   -1,
          LEARN_DOWN             ENT_LEARN,      LP_LEARN,        -1,           -1,   STOPPED,          -1,           -1,              -1,        STOPPED,      -1,              -1,   -1

                            --- actions() ---                                                    --- events() ---        
*/

#include <Homie.hpp>
#include <Wire.h>
#include "SknGarageDoor.hpp"

#ifndef ARDUINO_ESP32_DEV
extern "C"
{
#include <user_interface.h>
}
#endif

#define SKN_MOD_NAME "Garage Door Automation"
#define SKN_MOD_VERSION "1.0.3"
#define SKN_MOD_BRAND "SknSensors"

#define SKN_TITLE "Garage Door"
#define SKN_TYPE "Rollershutter"
#define SKN_ID "SknGarageDoor"

#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
// D1_mini Pins
  #define SDA 4        // D2
  #define SCL 5        // D1
  #define LOX_GPIO   13    // D7
  #define RELAY_GPIO 12    // D6
  #undef LED_BUILTIN
  #define LED_BUILTIN 2
#elif defined(ARDUINO_ESP8266_ESP12)
// Esp8266EX  pins
  #define SDA 5
  #define SCL 4
  #define LOX_GPIO   13    // D7
  #define RELAY_GPIO 12    // D6
  #undef LED_BUILTIN
  #define LED_BUILTIN 2
#elif defined(ARDUINO_ESP8266_NODEMCU)
// nodemcu esp12f  pins  -- ESP8266 Relay Module, ESP-12F Development Board AC 220V DC 12V Single relay module
  #define SDA        14
  #define SCL        12
  #define LOX_GPIO   13    
  #define RELAY_GPIO  5   
  #undef LED_BUILTIN
  #define LED_BUILTIN 16
#elif defined(ARDUINO_ESP32_DEV)
// esp32doit-devkit-v1  pins
  #define SDA 21
  #define SCL 22
  #define ATM_PLAYER_DISABLE_TONE   // disable some Automaton Ardunio issues (noTone, Tone, AnalogWrite)
  #define LOX_GPIO   13    // D7
  #define RELAY_GPIO 12    // D6
  #undef LED_BUILTIN
  #define LED_BUILTIN 4
#endif

 /* *
  *
  * guard-flag to prevent sending properties when mqtt is offline
  * 
*/
volatile bool gbEnableDoorOperations=false;

/* 
 * Homie Nodes */
SknGarageDoor doorNode(SKN_ID, SKN_TITLE, SKN_TYPE, LOX_GPIO, RELAY_GPIO); // communication interface

/**
 * look for events that block sending property info */
void onHomieEvent(const HomieEvent& event) {
  switch (event.type) {
    case HomieEventType::MQTT_READY:
      Serial << "MQTT connected" << endl;
      gbEnableDoorOperations=true;
      break;
    case HomieEventType::MQTT_DISCONNECTED:
      Serial << "MQTT disconnected, reason: " << (int8_t)event.mqttReason << endl;
      gbEnableDoorOperations=false;
      break;
    case HomieEventType::SENDING_STATISTICS:
      Serial << "Sending statistics" << endl;
      doorNode.updateDoorInfo();
      break;
  }
}

/*
 * Callback for Homie Broadcasts */
bool broadcastHandler(const String &level, const String &value)
{
  Homie.getLogger() << "Received broadcast level " << level << ": " << value << endl;
  return true;
}

/*
 * Arduino Setup: Initialze Homie */
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
      .disableResetTrigger()
      .onEvent(onHomieEvent);

  Homie.setup();
}

/*
 * Arduino Loop: Cycles Homie Nodes */
void loop()
{
  Homie.loop();
}
