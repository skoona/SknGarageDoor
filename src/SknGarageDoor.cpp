/**
 * Homie Node for SknGarageDoor
 *
 */
#include "SknGarageDoor.hpp"

SknGarageDoor::SknGarageDoor(const char *id, const char *name, const char *cType, int rangerReadyPin, int relayPin) 
    : HomieNode(id, name, cType),
    dataReadyPin(rangerReadyPin),
    ranger(),                // measures distance of door
    door(relayPin, ranger),  // controls door relay and startng stopping of ranger
    irq()                    // handles data ready interrupt for ranger
{
   printCaption();
}

/**
 *
 */
void SknGarageDoor::printCaption() {
  Homie.getLogger() << cCaption << " " << getId() << endl;
}

/**
 *
 */
void SknGarageDoor::updateDoorInfo() {
  if(gbEnableDoorOperations) {
    setProperty(cSknDoorID).send(cDoorState);
    setProperty(cSknPosID).send(String(iDoorPosition));
    if(bDoorAutoLearnActive) {
    setProperty(cSknModeID).send(cAutoLearnDoorState);
    }
  }
}

/**
 * Handles the received MQTT messages from Homie.
 *
 */
bool SknGarageDoor::handleInput(const HomieRange& range, const String& property, const String& value) {
  bool rc = false;
  Homie.getLogger() << cIndent << "〽 handleInput -> property '" << property << "' value=" << value << endl;

  // Control the Door
  if (property.equalsIgnoreCase(cSknPosID))
  {
    if (isDigit(value.charAt(0))) {
      uint8_t perValue = constrain(value.toInt(), 0, 100);
      door.cmd_pos(perValue);
      rc = true;

    } else if (value.equalsIgnoreCase("up")) {
      door.cmd_up();
      rc = true;

    } else if (value.equalsIgnoreCase("down")) {
      door.cmd_down();
      rc = true;

    } else if (value.equalsIgnoreCase("stop")) {
      door.cmd_stop();
      rc = true;
    } 

    if(rc) {
      updateDoorInfo();
    }

    // Node Services
  } else if(property.equalsIgnoreCase(cSknModeID)) {
    if(value.equalsIgnoreCase("reboot")) {
      Homie.getLogger() << cIndent << "〽 RESTARTING OR REBOOTING MACHINE ";
      cAutoLearnDoorState =  "Rebooting in 5 seconds";
      ESP.restart();
      rc = true;
    } else if (value.equalsIgnoreCase("auto_learn_up")) {
      Homie.getLogger() << cIndent << "〽 Auto Learn Up ";
      bDoorAutoLearnActive=true;
      cAutoLearnDoorState =  "Auto Learn Up";
      door.cmd_auto_learn_up();
      rc = true;
    } else if (value.equalsIgnoreCase("auto_learn_down")) {
      Homie.getLogger() << cIndent << "〽 Auto Learn Down ";
      bDoorAutoLearnActive=true;
      cAutoLearnDoorState =  "Auto Learn Down";
      door.cmd_auto_learn_down();
      rc = true;
    }    
    
    if(rc) {
      updateDoorInfo();
    }
  }

  return rc;
}


/**
 *
 */
void SknGarageDoor::onReadyToOperate() {
  enableAutomatons();

  Homie.getLogger()
      << "〽 "
      << "Node: " << getName()
      << " Ready to operate: " 
      << cDoorState
      << "("
      << iDoorPosition
      << ") Auto Learn: "
      << cAutoLearnDoorState
      << endl;
  updateDoorInfo();
}

/**
 *
 */
void SknGarageDoor::setDoorState_cb(char *_state) {
  cDoorState = _state;
  updateDoorInfo();
}
/**
 *
 */
void SknGarageDoor::setDoorPosition_cb(unsigned int _position, unsigned int _requested) {
  iDoorPosition = (int)_position;
  updateDoorInfo();
}

 /*
  * Save door limits */
bool SknGarageDoor::autoLearnSaveLimits(){
  bool rc = false;
  prefs.begin(cSknDoorID, false);
      prefs.putInt("mm_min", iDoorAutoLearnUpPosition);
      prefs.putInt("mm_max", iDoorAutoLearnDownPosition);
      rc = prefs.isKey("mm_min");
  prefs.end();
  Homie.getLogger()
      << "〽 "
      << "Node: " << getName()
      << " Auto Learn Limits Saving:[" 
      << (rc ? "True": "False")
      << "] up="
      << iDoorAutoLearnUpPosition
      << ", down="
      << iDoorAutoLearnDownPosition
      << endl;
  return rc;
}

 /*
  * Restore door limits */
bool SknGarageDoor::autoLearnRestoreLimits(){
  bool rc = false;
  prefs.begin(cSknDoorID, false);
  if (prefs.isKey("mm_min")) {
    iDoorAutoLearnUpPosition = prefs.getInt("mm_min", MM_MIN);
    iDoorAutoLearnDownPosition = prefs.getInt("mm_max", MM_MAX);
    rc = true;
  }
  prefs.end();
  Homie.getLogger()
      << "〽 "
      << "Node: " << getName()
      << " Auto Learn Limits Restoring:[" 
      << (rc ? "True": "False")
      << "] up="
      << iDoorAutoLearnUpPosition
      << ", down="
      << iDoorAutoLearnDownPosition
      << endl;
  return rc;
}


/**
 * @brief 
 * 
 */
void SknGarageDoor::enableAutomatons() {
  if(!vbOne) {
    vbOne=true;

    autoLearnRestoreLimits();

    ranger.begin( 1024);       // vl53l1x Time of Flight distance measurement

    irq.begin(dataReadyPin, 30, true, true) // ranger interrupt pin when data ready
	    .onChange(HIGH, [this]( int idx, int v, int up ) { 
        long posValue;
        long mmPos = (long)ranger.readValues(true);
        posValue =constrain( map(mmPos, iDoorAutoLearnUpPosition, iDoorAutoLearnDownPosition, 0, 100), 0, 100);
        if (bDoorAutoLearnActive ) {
          if(door.state() == SknAtmDoor::LEARN_UP) {
            if(mmPos<(MM_MIN/2)) mmPos = MM_MIN;
            iDoorAutoLearnUpPosition=mmPos;
            snprintf(cAutoBuffer, sizeof(cAutoBuffer), "Auto Learn up: %ld mm", mmPos);
            cAutoLearnDoorState = cAutoBuffer;
          } else if(door.state() == SknAtmDoor::LEARN_DOWN) {
            if(mmPos<=MM_MIN) mmPos = MM_MAX;
            iDoorAutoLearnDownPosition=mmPos;
            snprintf(cAutoBuffer, sizeof(cAutoBuffer), "Auto Learn Down: %ld mm", mmPos);
            cAutoLearnDoorState = cAutoBuffer;
          }
          posValue =constrain( map(mmPos, MM_MIN, MM_MAX, 0, 100), 0, 100);
        }
        door.setDoorPosition_cb( posValue );
        iDoorPosition = posValue; // save local value
      }, 0);

    door.begin()                  // door relay and operational logic
      .trace( Serial )
      .onChange([this]( int idx, int v, int up ) { 
        if(bDoorAutoLearnActive && v<SknAtmDoor::LEARN_UP) {
          autoLearnSaveLimits();
          snprintf(cAutoBuffer, sizeof(cAutoBuffer), "Auto Learn Range, Up %d mm, Down %d mm", iDoorAutoLearnUpPosition, iDoorAutoLearnDownPosition);
          cAutoLearnDoorState = cAutoBuffer;
          bDoorAutoLearnActive=false;
          setProperty(cSknModeID).send(cAutoLearnDoorState);
        }
        setDoorState_cb((char *)door.mapstate(v));
        if(v==SknAtmDoor::STOPPED) {
          snprintf(cAutoBuffer, sizeof(cAutoBuffer), "Auto Learn Range, Up %d mm, Down %d mm", iDoorAutoLearnUpPosition, iDoorAutoLearnDownPosition);
          cAutoLearnDoorState = cAutoBuffer;
          setProperty(cSknModeID).send(cAutoLearnDoorState);
        }
      },0)
	    .onPos([this]( int idx, int v, int up ) { 
        setDoorPosition_cb(v,up); 
      },0);


    ranger.start(); // collect 5ish positions on init
      irq.cycle(5000); // load Rangers average counter
      door.cycle(512); // let door process one or two positions
    ranger.stop();   

  }
}

/**
 *
 */
void SknGarageDoor::setup() {

   advertise(cSknDoorID)
    .setName("State")
    .setDatatype("enum")
    .setFormat("STOPPED,MOVING_UP,UP,MOVING_DOWN,DOWN,MOVING_POS")
    .setRetained(true);

  advertise(cSknPosID)
    .setName("Position")
    .setDatatype("integer")
    .setFormat("0:100")
    .setUnit("%")
    .setRetained(true)
    .settable();
    // Commands: digits:0-100, UP, DOWN, STOP

  snprintf(cAutoBuffer, sizeof(cAutoBuffer), "Auto Learn Range, Up %d mm, Down %d mm", iDoorAutoLearnUpPosition, iDoorAutoLearnDownPosition);
  cAutoLearnDoorState = cAutoBuffer;
  advertise(cSknModeID)
    .setName("Services")
    .setDatatype("string")
    .setFormat("")
    .settable();
    // Commands: auto_learn_up, auto_learn_down, reboot

  Homie.getLogger()
      << "〽 "
      << "Node: " << getName()
      << " SknGarageDoor::setup() "
      << endl;
}

/**
 *
 */
void SknGarageDoor::loop() {
  if(vbOne && gbEnableDoorOperations) {
    automaton.run();
  }
}
