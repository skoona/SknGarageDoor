/**
 * Homie Node for SknGarageDoor
 *
 */
#include "SknGarageDoor.hpp"

SknGarageDoor::SknGarageDoor(const char *id, const char *name, const char *cType, SknLoxRanger& rangerObj) 
    : HomieNode(id, name, cType),
    ranger(rangerObj)
{
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

  Homie.getLogger() << cIndent << "〽 handleInput -> property '" << property << "' value=" << value << endl;

  if (property.equalsIgnoreCase(cSknPosID))
  {
    if (isDigit(value.charAt(0))) {
      uint8_t perValue = value.toInt();
			if (perValue > 100) return false;
      ranger.start();
      setProperty(cSknPosID).send(String(perValue));
      // DO SOMETHING
    } else if (value.equalsIgnoreCase("up")) {
      ranger.start();
      Homie.getLogger() << cIndent << "Door is UP" << endl;
      setProperty(cSknDoorID).send("UP");
      setProperty(cSknPosID).send("0");

    } else if (value.equalsIgnoreCase("down")) {
      ranger.start();
      Homie.getLogger() << cIndent << "Door is DOWN" << endl;
      setProperty(cSknDoorID).send("DOWN");
      setProperty(cSknPosID).send("100");

    } else if (value.equalsIgnoreCase("stop")) {
      ranger.stop();
      Homie.getLogger() << cIndent << "Door is STOPPED" << endl;
      setProperty(cSknDoorID).send("STOPPED");
      setProperty(cSknPosID).send("40");

    } else {
      Homie.getLogger() << cIndent << "Door is IDLE at position " << uiDistanceValue << endl;
      setProperty(cSknDoorID).send("IDLE");
      setProperty(cSknPosID).send("100");
    }

    return true;
  }

  return false;
}

/**
 *
 */
void SknGarageDoor::loop() {
  automaton.run();
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
void SknGarageDoor::setDoorPosition(unsigned int _position) {
  uiDistanceValue = _position;
  setProperty(cSknPosID).send(String(uiDistanceValue));
}

/**
 *
 */
void SknGarageDoor::setup() {
  
  if(vbOne) {
    ranger.begin(13, 1000);
      // .rangerStart();

    irq.begin(13, 30, true, true)
      .trace( Serial )
      .onChange(HIGH, readDoorPositionCallback );

    vbOne=false;
  }
}
