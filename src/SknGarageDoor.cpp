/**
 * Homie Node for SknGarageDoor
 *
 */
#include "SknGarageDoor.hpp"

SknGarageDoor::SknGarageDoor(const char *id, const char *name, const char *cType, int rangerReadyPin, SknAtmDigital& irqObj, SknLoxRanger& rangerObj, SknAtmDoor& doorObj) 
    : HomieNode(id, name, cType),
    dataReadyPin(rangerReadyPin),
    ranger(rangerObj),
    door(doorObj),
    irq(irqObj)
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
 * Handles the received MQTT messages from Homie.
 *
 */
bool SknGarageDoor::handleInput(const HomieRange& range, const String& property, const String& value) {
  bool rc = false;
  Homie.getLogger() << cIndent << "〽 handleInput -> property '" << property << "' value=" << value << endl;

  if (property.equalsIgnoreCase(cSknPosID))
  {
    if (isDigit(value.charAt(0))) {
      uint8_t perValue = value.toInt();
			if (perValue <= 100) {
        door.cmd_pos(perValue);
        rc = true;
      }

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
      setProperty(cSknDoorID).send(cSknDoorState);
      setProperty(cSknPosID).send(String(iDoorPosition));
    }
  }

  return rc;
}

/**
 *
 */
void SknGarageDoor::onReadyToOperate() {
  Homie.getLogger()
      << "〽 "
      << "Node: " << getName()
      << " Ready to operate."
      << endl;
}

/**
 *
 */
void SknGarageDoor::setDoorState(char *_state) {
  cSknDoorState = _state;
  Homie.getLogger()
      << "〽 "
      << "Node: " << getName()
      << " Door State " << cSknDoorState
      << endl;  
  setProperty(cSknDoorID).send(cSknDoorState);
}
/**
 *
 */
void SknGarageDoor::setDoorPosition(unsigned int _position) {
  iDoorPosition = (int)_position;
  Homie.getLogger()
      << "〽 "
      << "Node: " << getName()
      << " Door Position " << iDoorPosition
      << endl;
  setProperty(cSknPosID).send(String(iDoorPosition));
}

/**
 * @brief 
 * 
 */
void SknGarageDoor::enableAutomatons() {
  if(!vbOne) {
    ranger.begin( 1000);       // vl53l1x line of sight distance measurement

    irq.begin(dataReadyPin, 30, true, true) // ranger interrupt pin when data ready
      // .trace( Serial )
	    .onChange(HIGH, [this]( int idx, int v, int up ) { 
          long posValue =constrain( map((long)ranger.readValues(false), 10, 1960, 0, 100), 0, 100);
          door.setDoorPosition( posValue );
          setDoorPosition( posValue );
          Serial.printf("[MAIN]collectDoorPosition() Door position = %ld, idx=%d, v=%d, up=%d\n", posValue, idx,v,up);
        }, 0);

    door.begin()                  // door relay and operational logic
      .trace( Serial )
      .onChange([this]( int idx, int v, int up ) { setDoorState((char *)door.mapstate(v));},0)
	    .onPos([this]( int idx, int v, int up ) { setDoorPosition(v); },0);

    ranger.start(); // collect 5ish positions on init
    irq.cycle(5000);
    ranger.stop();

    vbOne=true;
  }
}
/**
 *
 */
void SknGarageDoor::setup() {
   advertise(cSknDoorID)
    .setName("State")
    .setDatatype("enum")
    .setRetained(true);
    // .setFormat("IDLE,DOWN,MOVING_UP,UP,MOVING_DOWN,STOPPED")

  advertise(cSknPosID)
    .setName("Position")
    .setDatatype("integer")
    .setUnit("%")
    .setFormat("0:100")
    .settable();
    // Commands: digits:0-100, UP, DOWN, STOP

  enableAutomatons();

  Homie.getLogger()
      << "〽 "
      << "Node: " << getName()
      << " SknGarageDoor::setup() " << vbOne
      << endl;
}

/**
 *
 */
void SknGarageDoor::loop() {
  if(vbOne) {
    automaton.run();
  }
}
