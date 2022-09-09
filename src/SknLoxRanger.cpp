/**
 * Homie Node for VL53L1x.
 *
 */
#include "SknLoxRanger.hpp"

SknLoxRanger::SknLoxRanger( const int gpioPin)
    : _pinGPIO(gpioPin      
{
  printCaption();

  pinMode(_pinGPIO, INPUT_PULLUP);

  vbEnabled = false;
  unsigned long int time_now = millis();

  lox.setTimeout(500);
  if (!lox.init())
  {
    // delay(1000); // Delay is not working in this class ???
    time_now = millis();
    while (millis() < time_now + 1000){}

    while (!lox.init())
    {
      Homie.getLogger() << cIndent << "Failed to detect and initialize sensor!" << endl;
      // delay(1000 );
      time_now = millis();
      while (millis() < time_now + 1000){}
    }
  }

  if (lox.setDistanceMode(VL53L1X::Medium))
  {
    Homie.getLogger() << "〽 Medium distance mode accepted." << endl;
  }

  if (lox.setMeasurementTimingBudget(200000))
  {
    Homie.getLogger() << "〽 200ms timing budget accepted." << endl;

    ulLastTimebase = millis();
  }

}

/**
 *
 */
void SknLoxRanger::setOpenThresholdMM(const int mm) {
  thresholdOpen = mm;
}
/**
 *
 */
void SknLoxRanger::setClosedThresholdMM(const int mm) {
  thresholdClosed = mm;
}


/**
 *
 */
void SknLoxRanger::setDirectionStatus(const int value)
{
  switch(value) {
    case IDLE:
      if (distances[MAX_SAMPLES] >= thresholdClosed)
      {
        strcpy(cDirection, "CLOSED");
      }
      else if (distances[MAX_SAMPLES] <= thresholdOpen)
      {
        strcpy(cDirection, "OPEN");
      }
      else
      {
        strcpy(cDirection, "IDLE");
      }
      break;
    case OPENING:
      strcpy(cDirection, "OPENING");
      break;
    case CLOSING:
      strcpy(cDirection, "CLOSING");
      break;
    default:
      strcpy(cDirection, "IDLE");
  }
}

/**
 *
 */
bool SknLoxRanger::isReady()
{
  return vbEnabled;
}

/**
 *
 */
bool SknLoxRanger::isOpen()
{
  String value = String(cDirection);

  if (!(value.equals("CLOSED")) && isReady())
  {
    return true;
  }
  return false;
}

/**
 *
 */
bool SknLoxRanger::isClosed()
{
  String value = String(cDirection);

  if (value.equals("CLOSED") && isReady())
  {
    return true;
  }
  return false;
}

/**
 *
 */
void SknLoxRanger::stopRanging() {
  lox.stopContinuous();  
}


/**
 *
 */
void SknLoxRanger::startRanging(uint32_t duration) {
      lox.startContinuous(duration);
}

/**
 *
 */
void SknLoxRanger::printCaption() {
  Homie.getLogger() << cCaption << "  " << getId() << endl;
}

/**
 * @brief Collect distance and determine direction of travel
 *
 */
unsigned int SknLoxRanger::handleLoxRead()
{
  const int capacity = (MAX_SAMPLES);
  int idleUpDown = 0;

  unsigned int value = (unsigned int)lox.read(false);
  if (value == 0) {
    return uiDistanceValue;
  }

  for (int idx = 0; idx < capacity; idx++) {
    distances[idx] = distances[idx+1]; // move all down
  }

  if (lox.ranging_data.range_status == 0) {
    distances[capacity] = lox.ranging_data.range_mm;
    uiDistanceValue = lox.ranging_data.range_mm;
  } else {
    distances[capacity] = uiDistanceValue;
  }

  if (distances[0] > 0) {
    idleUpDown = (distances[0] - distances[capacity]);

    if (abs(idleUpDown) < 20 )
    {
      idleUpDown = IDLE;  // idle
    }
    else if (idleUpDown >= 60)
    {
      idleUpDown = OPENING; // opening
    }
    else
    {
      idleUpDown = CLOSING; // closing
    }

    setDirectionStatus(idleUpDown);
  }

  return uiDistanceValue;
}

/**
 *
 */
bool SknLoxRanger::collectRangeValues()
{
  if (!digitalRead(_pinGPIO))
  {
    handleLoxRead();
    char buf[32];
    snprintf(buf, sizeof(buf), cRangeFormat, lox.ranging_data.range_mm);
    setProperty(cRangeID).send(buf);

    snprintf(buf, sizeof(buf), cStatusFormat, lox.rangeStatusToString(lox.ranging_data.range_status));
    setProperty(cStatusID).send(buf);

    snprintf(buf, sizeof(buf), cSignalFormat, lox.ranging_data.peak_signal_count_rate_MCPS);
    setProperty(cSignalID).send(buf);

    snprintf(buf, sizeof(buf), cAmbientFormat, lox.ranging_data.ambient_count_rate_MCPS);
    setProperty(cAmbientID).send(buf);

    setProperty(cDirectionID).send(cDirection);

    Homie.getLogger() << "〽 range: " << lox.ranging_data.range_mm
                      << " mm \tstatus: " << lox.rangeStatusToString(lox.ranging_data.range_status)
                      << " raw: " << lox.ranging_data.range_status
                      << "\tsignal: " << lox.ranging_data.peak_signal_count_rate_MCPS
                      << " MCPS\tambient: " << lox.ranging_data.ambient_count_rate_MCPS
                      << " MCPS"
                      << " Direction: " << cDirection << endl;
  return true;                      
  }

  return false;
}

/**
 *
 */
// void SknLoxRanger::setup() {

//   advertise(cRangeID)
//           .setName("distance in mm")
//           .setDatatype("integer")
//           .setFormat(cRangeFormat)
//           .setRetained(true)
//           .setUnit("mm");

//   advertise(cStatusID)
//       .setName("range operating status")
//       .setDatatype("string")
//       .setFormat(cStatusFormat)
//       .setRetained(true)
//       .setUnit("#");

//   advertise(cSignalID)
//       .setName("peak signal count rate")
//       .setDatatype("float")
//       .setFormat(cSignalFormat)
//       .setRetained(false)
//       .setUnit("mcps");

//   advertise(cAmbientID)
//       .setName("ambient light count rate")
//       .setDatatype("float")
//       .setFormat(cAmbientFormat)
//       .setRetained(false)
//       .setUnit("mcps");

//   advertise(cDirectionID)
//       .setName("Direction of movement")
//       .setDatatype("enum")
//       .setFormat(cDirectionFormat)
//       .setRetained(false);

//   advertise(cOperateID)
//       .setName("Actively Ranging")
//       .setDatatype("enum")
//       .setFormat(cOperateFormat)
//       .setRetained(false);
// }
