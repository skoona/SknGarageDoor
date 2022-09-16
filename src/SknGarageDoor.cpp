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
      setProperty(cSknDoorID).send(cDoorState);
      setProperty(cSknPosID).send(String(iDoorPosition));
    }

    // Restart this node
  } else if(property.equalsIgnoreCase(cSknRestartID) && value.equalsIgnoreCase("now")) {
    Homie.getLogger() << cIndent << "〽 RESTARTING OR REBOOTING MACHINE ";
    setProperty(cSknRestartID).send("Rebooting in 5 seconds");
    ESP.restart();
    rc = true;
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
      << ")"
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

/**
 * @brief 
 * 
 */
void SknGarageDoor::enableAutomatons() {
  if(!vbOne) {
    vbOne=true;

    ranger.begin( 1024);       // vl53l1x Time of Flight distance measurement

    irq.begin(dataReadyPin, 30, true, true) // ranger interrupt pin when data ready
	    .onChange(HIGH, [this]( int idx, int v, int up ) { 
        long posValue =constrain( map((long)ranger.readValues(true), MM_MIN, MM_MAX, 0, 100), 0, 100);
        door.setDoorPosition_cb( posValue );
        iDoorPosition = posValue; // save local value
      }, 0);

    door.begin()                  // door relay and operational logic
      .trace( Serial )
      .onChange([this]( int idx, int v, int up ) { 
        setDoorState_cb((char *)door.mapstate(v));
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

  advertise(cSknRestartID)
    .setName("Reboot")
    .setDatatype("string")
    .setFormat("")
    .settable();
    // Commands: now

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
