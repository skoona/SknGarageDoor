/**
 * Automaton Node for SknAtmDoor
 *
 */
#include "SknAtmDoor.hpp"

SknAtmDoor::SknAtmDoor(uint8_t relayPin, SknLoxRanger& lox) 
    : Machine(),
    uiRelayPin(relayPin),
    ranger(lox)
 { 
    pinMode(uiRelayPin, OUTPUT); // Door operator
    digitalWrite(uiRelayPin, LOW); // Init door to off
    uiEstimatedPosition = 100;
    uiRequestedPosition = 100;
    on = false;
};

SknAtmDoor& SknAtmDoor::begin()
{
     // clang-format off
    const static state_t state_table[] PROGMEM = {
        /*                             ON_ENTER       ON_LOOP    ON_EXIT     EVT_STEP        EVT_DOWN      EVT_STOP        EVT_UP  EVT_POS_REACHED  ELSE */
        /*     STOPPED */           ENT_STOPPED,           -1,        -1,          -1,    MOVING_DOWN,           -1,    MOVING_UP,              -1,   -1,
        /*   MOVING_UP */         ENT_MOVING_UP,       LP_POS,        -1,          UP,    MOVING_DOWN,      STOPPED,           -1,              UP,   -1,
        /*          UP */                ENT_UP,           -1,        -1,          -1,    MOVING_DOWN,      STOPPED,           -1,              -1,   -1,
        /* MOVING_DOWN */       ENT_MOVING_DOWN,       LP_POS,        -1,        DOWN,             -1,      STOPPED,    MOVING_UP,            DOWN,   -1,
        /*        DOWN */              ENT_DOWN,           -1,        -1,          -1,             -1,      STOPPED,    MOVING_UP,              -1,   -1,
    };
    // clang-format on
    Machine::begin(state_table, ELSE);

    return *this;
}

/*
 * Relay management
*/
SknAtmDoor& SknAtmDoor::relayPause(unsigned int _ms) {
    unsigned long int time_delay = millis() + _ms;
    while (millis() < time_delay){}
    return *this;
}
SknAtmDoor& SknAtmDoor::relayStart() {
    digitalWrite(uiRelayPin, HIGH);
    relayPause(384); 
    digitalWrite(uiRelayPin, LOW); 
    return *this;    
}
SknAtmDoor& SknAtmDoor::relayStop() {
    digitalWrite(uiRelayPin, HIGH);
    relayPause(64); 
    digitalWrite(uiRelayPin, LOW); 
    return *this;    
}
SknAtmDoor& SknAtmDoor::relayChangeDirection() {
    // relayStop().relayPause(10).relayStart();        HOLD FOR LATER EVALUATION
    Serial.printf("^ SknAtmDoor::relayChangeDirection() ep=%d, rp=%d, eReq:%s, state:%s\n", uiEstimatedPosition, uiRequestedPosition, mapstate(eRequestedDirection), mapstate(state()));
    return *this;    
}

/*
 *  external effects Action helpers
*/
void SknAtmDoor::moveUp() { 
    bUpOrDown=true;
    ranger.start();
    relayStart();
}
void SknAtmDoor::moveDn() { 
    bUpOrDown=false;
    ranger.start(); 
    relayStart();
}
void SknAtmDoor::moveStp() { 
    relayStop();
    ranger.stop(); 
}
void SknAtmDoor::moveHalt() { 
    ranger.stop(); 
}
void SknAtmDoor::moveChgDir() { 
    relayChangeDirection();
}

/* Add C++ code for each internally handled event (input)
 * The code must return 1 to trigger the event
 * EVT_STEP, EVT_CMD_DOWN, EVT_CMD_STOP, EVT_CMD_UP, EVT_POS_REACHED, ELSE
 */
int SknAtmDoor::event(int id) {
    switch (id)
    {        
    case EVT_STEP:
        return (id == next_trigger);

    case EVT_DOWN: // dn=100      pos=50
        return (id == next_trigger);

    case EVT_STOP:
        return (id == next_trigger);

    case EVT_UP:   // up=0        pos=50
        return (id == next_trigger);

    case EVT_POS_REACHED:
        if( uiRequestedPosition == uiEstimatedPosition ) {
            // Serial.printf("[EVT_POS_REACHED]Pos %d reached, request was %d, eReq:%s, state:%s\n", uiEstimatedPosition, uiRequestedPosition, mapstate(eRequestedDirection), mapstate(state()));
            return true;
        }
    }
    return false;
}

/* Add C++ code for each action
 * This generates the 'output' for the state machine
 *
 * Available connectors:
 *   push( connectors, ON_CHANGE | ATM_BROADCAST, 1, <v>, <up> );
 *   push( connectors, ON_POS, 0, <v>, <up> );
 *   ENT_STOPPED, ENT_MOVING_UP, ENT_UP, ENT_MOVING_DOWN, ENT_DOWN, LP_POS  // ACTIONS
 */
void SknAtmDoor::action(int id)
{
    switch (id)
    {
    case ATM_ON_SWITCH:
    Serial.printf("[action]ATM_ON_SWITCH id %d, nextTrigger %d, current %s, next %s, eReq:%s\n", 
        id, next_trigger, 
        mapstate(current),  mapstate(next), mapstate(eRequestedDirection));
        break;
    case ENT_STOPPED:
        moveHalt();
        // uiEstimatedPosition = uiRequestedPosition;
        push(connectors, ON_POS, 0, uiEstimatedPosition, 0);
        push(connectors, ON_CHANGE, 0, state(), 0);
        break;
    case ENT_MOVING_UP:
        moveUp();
        push(connectors, ON_CHANGE, 0, state(), 0);
        if (uiEstimatedPosition % 2 == 0)
            push(connectors, ON_POS, 0, uiEstimatedPosition, 0);
        break;
    case ENT_UP:
        moveHalt();
        push(connectors, ON_CHANGE, 0, state(), 0);
        break;
    case ENT_MOVING_DOWN:
        moveDn();
        push(connectors, ON_CHANGE, 0, state(), 0);
        if (uiEstimatedPosition % 2 == 0)
            push(connectors, ON_POS, 0, uiEstimatedPosition, 0);
        break;
    case ENT_DOWN:
        moveHalt();
        push(connectors, ON_CHANGE, 0, state(), 0);
        break;
    case LP_POS:
        if(uiRequestedPosition==uiEstimatedPosition) {
            trigger(EVT_STEP);
        }
        // Serial.printf("^ SknAtmDoor::Action() ep=%d, rp=%d, eReq:%s, state:%s\n", uiEstimatedPosition, uiRequestedPosition, mapstate(eRequestedDirection), mapstate(state()));
        break;
    }
}

/* Optionally override the default trigger() method
 * Control how your machine processes triggers
 */

SknAtmDoor& SknAtmDoor::trigger(int event) {
    Machine::trigger(event);
    return *this;
}

/* Optionally override the default state() method
 * Control what the machine returns when another process requests its state
 */

int SknAtmDoor::state(void) {
    return Machine::state();
}

const char * SknAtmDoor::mapstate(int state) { 
    return mapSymbol(state + state_width - ATM_ON_EXIT, symbols); 
}

/* 
 * Public event methods
 */
SknAtmDoor& SknAtmDoor::cmd_pos(uint8_t destPos) {
    // sort out up/down direction and issue command
    // 100ish is down
    // 0ish is up
    uiRequestedPosition = destPos;
                //  100       0    
    int8_t delta = destPos - uiEstimatedPosition;
    if (delta > 0 || destPos == 100) {
        Serial.printf("%d steps to go up\n", delta);
        eRequestedDirection = MOVING_UP;
        trigger(EVT_UP);
    } else {
        Serial.printf("%d steps to go down\n", -delta);
        eRequestedDirection = MOVING_DOWN;
        trigger(EVT_DOWN);
    }
    return *this;
}

SknAtmDoor& SknAtmDoor::cmd_down() {
    uiRequestedPosition = 100;
    eRequestedDirection = MOVING_DOWN;
	trigger(EVT_DOWN);
	return *this;
}

SknAtmDoor& SknAtmDoor::cmd_stop() {
    uiRequestedPosition = uiEstimatedPosition;
    eRequestedDirection = STOPPED;
	trigger(EVT_STOP);
	return *this;
}

SknAtmDoor& SknAtmDoor::cmd_up() {
    uiRequestedPosition = 0;
    eRequestedDirection = MOVING_UP;
	trigger(EVT_UP);
	return *this;
}

SknAtmDoor& SknAtmDoor::setDoorPosition(uint8_t currentPosition) {
    uiEstimatedPosition = currentPosition;
    iSampleCount++;
    bool bDirection = false;
    for( iSamples = 0; iSamples < MAX_SAMPLES; iSamples++) { // shift samples down
        iaDirection[iSamples] = iaDirection[iSamples+1];
    }
    iaDirection[iSamples] = currentPosition; // assign latest value to top

    // are we moving up
    if((iSampleCount >= MAX_SAMPLES) && (iaDirection[0] > iaDirection[iSamples])) { // moving up 20 > 5 = UP[0]
        bDirection = true;
        if (eRequestedDirection != current) {
            moveChgDir();
            iSampleCount = 0;
        }

    } else { // moving down
        if (eRequestedDirection != current) {
            moveChgDir();
            iSampleCount = 0;
        }
    }

    Serial.printf("SknAtmDoor::setDoorPosition(%d) Position:%d, Moving:%s, sReq:%s, sCur:%s, vLow:%d, vHigh:%d\n", 
        iSampleCount, currentPosition, (bDirection ? "UP" : "DOWN"), 
        mapstate(eRequestedDirection), mapstate(state()), 
        iaDirection[0], iaDirection[iSamples]);

    return *this;
}

/*
 * onChange() push connector variants ( slots 1, autostore 0, broadcast 1 )
 */

SknAtmDoor& SknAtmDoor::onChange( Machine& machine, int event ) {
  onPush( connectors, ON_CHANGE, 0, 1, 1, machine, event );
  return *this;
}

SknAtmDoor& SknAtmDoor::onChange( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_CHANGE, 0, 1, 1, callback, idx );
  return *this;
}

SknAtmDoor& SknAtmDoor::onChange(atm_cb_lambda_t callback, int idx) {
	onPush(connectors, ON_CHANGE, 0, 1, 1, callback, idx);
	return *this;
}

/*
 * onPos() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

SknAtmDoor& SknAtmDoor::onPos( Machine& machine, int event ) {
  onPush( connectors, ON_POS, 0, 1, 1, machine, event );
  return *this;
}

SknAtmDoor& SknAtmDoor::onPos( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_POS, 0, 1, 1, callback, idx );
  return *this;
}

SknAtmDoor& SknAtmDoor::onPos( atm_cb_lambda_t callback, int idx ) {
  onPush( connectors, ON_POS, 0, 1, 1, callback, idx );
  return *this;
}

/* State trace method
 * Sets the symbol table and the default logging method for serial monitoring
 */
SknAtmDoor& SknAtmDoor::trace( Stream & stream ) {
  Machine::setTrace( &stream, atm_serial_debug::trace,
    "DOOR\0EVT_STEP\0EVT_DOWN\0EVT_STOP\0EVT_UP\0EVT_POS_REACHED\0ELSE\0STOPPED\0MOVING_UP\0UP\0MOVING_DOWN\0DOWN" );
  return *this;
}
