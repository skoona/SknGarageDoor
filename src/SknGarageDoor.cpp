/**
 * Homie Node for SknGarageDoor
 *
 */
#include "SknGarageDoor.hpp"

SknGarageDoor::SknGarageDoor(const char *id, const char *name, const char *cType)
    : HomieNode(id, name, cType)
{
  advertise(cSknGarageDoorID)
    .setName("State")
    .setDatatype("enum")
    .setRetained(true);
    // .setFormat("IDLE,DOWN,MOVING_UP,UP,MOVING_DOWN,STOPPED")

  advertise(cSknGarageDoorPositionID)
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

  if (property.equalsIgnoreCase(cSknGarageDoorPositionID))
  {
    if (isDigit(value.charAt(0))) {
      uint8_t perValue = value.toInt();
			if (perValue > 100) return false;
      setProperty(cSknGarageDoorPositionID).send(String(perValue));
      // DO SOMETHING
    } else if (value.equalsIgnoreCase("up")) {
      Homie.getLogger() << cIndent << "Door is UP" << endl;
      setProperty(cSknGarageDoorID).send("UP");
      setProperty(cSknGarageDoorPositionID).send("0");

    } else if (value.equalsIgnoreCase("down")) {
      Homie.getLogger() << cIndent << "Door is DOWN" << endl;
      setProperty(cSknGarageDoorID).send("DOWN");
      setProperty(cSknGarageDoorPositionID).send("100");

    } else if (value.equalsIgnoreCase("stop")) {
      Homie.getLogger() << cIndent << "Door is STOPPED" << endl;
      setProperty(cSknGarageDoorID).send("STOPPED");
      setProperty(cSknGarageDoorPositionID).send("40");

    } else {
      Homie.getLogger() << cIndent << "Door is IDLE" << endl;
      setProperty(cSknGarageDoorID).send("IDLE");
      setProperty(cSknGarageDoorPositionID).send("100");
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
void SknGarageDoor::setup() {
}
