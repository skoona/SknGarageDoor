/**
 * Automaton Node for SknAtmDoor
 *
 */
#include "SknAtmDoor.hpp"

SknAtmDoor::SknAtmDoor(uint8_t relayPin, SknLoxRanger &lox) : Machine(),
                                        uiEstimatedPosition(100),
                                        uiRequestedPosition(100),
                                        ranger(lox),
                                        uiRelayPin(relayPin),
                                        on(false){};

SknAtmDoor& SknAtmDoor::begin()
{
     // clang-format off
    const static state_t state_table[] PROGMEM = {
        /*                             ON_ENTER       ON_LOOP    ON_EXIT     EVT_STEP    EVT_CMD_DOWN  EVT_CMD_STOP    EVT_CMD_UP  EVT_POS_REACHED  ELSE */
        /*     STOPPED */           ENT_STOPPED,           -1,        -1,          -1,    MOVING_DOWN,           -1,    MOVING_UP,              -1,   -1,
        /*   MOVING_UP */         ENT_MOVING_UP,       LP_POS,        -1,          -1,    MOVING_DOWN,      STOPPED,           -1,              UP,   -1,
        /*          UP */                ENT_UP,           -1,        -1,          -1,    MOVING_DOWN,           -1,           -1,              -1,   -1,
        /* MOVING_DOWN */       ENT_MOVING_DOWN,       LP_POS,        -1,          -1,             -1,      STOPPED,    MOVING_UP,            DOWN,   -1,
        /*        DOWN */              ENT_DOWN,           -1,        -1,          -1,             -1,           -1,    MOVING_UP,              -1,   -1,
    };
    // clang-format on
    Machine::begin(state_table, ELSE);
    pinMode(uiRelayPin, OUTPUT); // Door operator
    digitalWrite(uiRelayPin, LOW); // Init door to off

    return *this;
}

/*
 * Relay management
*/
SknAtmDoor& SknAtmDoor::relayStart() {
    digitalWrite(uiRelayPin, HIGH);
    delay(384); 
    digitalWrite(uiRelayPin, LOW); 
    return *this;    
}
SknAtmDoor& SknAtmDoor::relayStop() {
    digitalWrite(uiRelayPin, HIGH);
    delay(64); 
    digitalWrite(uiRelayPin, LOW); 
    return *this;    
}
SknAtmDoor& SknAtmDoor::relayChangeDirection() {
    relayStop().relayStart();
    return *this;    
}

/*
 * Action helpers
*/
void SknAtmDoor::moveUp() { 
    ranger.start();
    relayStart();
}
void SknAtmDoor::moveDn() { 
    ranger.start(); 
    relayStart();
}
void SknAtmDoor::moveStp() { 
    relayStop();
    ranger.stop(); 
}
void SknAtmDoor::moveChgDir() { 
    relayChangeDirection();
}

/* Add C++ code for each internally handled event (input)
 * The code must return 1 to trigger the event
 * EVT_STEP, EVT_CMD_DOWN, EVT_CMD_STOP, EVT_CMD_UP, EVT_POS_REACHED, ELSE
 */
int SknAtmDoor::event(int id)
{
    switch (id)
    {
    case EVT_POS_REACHED:
        if (counter_stepsToGo.expired() && !(uiEstimatedPosition == 0 || uiEstimatedPosition == 100))
        {
            Serial.printf("Pos %d reached.\n", uiEstimatedPosition);
            return true;
        }
        else
        {
            return false;
        }
    case EVT_CMD_UP:
    case EVT_CMD_STOP:
    case EVT_CMD_DOWN:
    case EVT_STEP:
        return false;
    }
    return 0;
}

/* Add C++ code for each action
 * This generates the 'output' for the state machine
 *
 * Available connectors:
 *   push( connectors, ON_CHANGE | ATM_BROADCAST, 1, <v>, <up> );
 *   push( connectors, ON_POS, 0, <v>, <up> );
 */
void SknAtmDoor::action(int id)
{
    switch (id)
    {
    case ENT_DOWN:
        moveStp();
        push(connectors, ON_CHANGE, 0, state(), 0);
        return;
    case ENT_MOVING_DOWN:
        moveDn();
        push(connectors, ON_CHANGE, 0, state(), 0);
        counter_stepsToGo.decrement();
        Serial.printf("Still %d steps to go down\n", counter_stepsToGo.value);
        uiEstimatedPosition++;
        if (uiEstimatedPosition % 2 == 0)
            push(connectors, ON_POS, 0, uiEstimatedPosition, 0);
        return;
    case ENT_STOPPED:
        moveStp();
        push(connectors, ON_POS, 0, uiEstimatedPosition, 0);
        push(connectors, ON_CHANGE, 0, state(), 0);
        return;
    case LP_POS:
        Serial.printf("^%u\n", uiEstimatedPosition);
        return;
    case ENT_MOVING_UP:
        moveUp();
        push(connectors, ON_CHANGE, 0, state(), 0);
        counter_stepsToGo.decrement();
        Serial.printf("Still %d steps to go up\n", counter_stepsToGo.value);
        uiEstimatedPosition--;
        if (uiEstimatedPosition % 2 == 0)
            push(connectors, ON_POS, 0, uiEstimatedPosition, 0);
        return;
    case ENT_UP:
        push(connectors, ON_CHANGE, 0, state(), 0);
        moveStp();
        return;
    }
}

/* Optionally override the default trigger() method
 * Control how your machine processes triggers
 */

SknAtmDoor& SknAtmDoor::trigger(int event)
{
    Machine::trigger(event);
    return *this;
}

/* Optionally override the default state() method
 * Control what the machine returns when another process requests its state
 */

int SknAtmDoor::state(void)
{
    return Machine::state();
}

const char * SknAtmDoor::mapstate(int state) { 
    return mapSymbol(state + state_width - ATM_ON_EXIT, symbols); 
}

/*
 *
 */
SknAtmDoor& SknAtmDoor::cmd_pos(uint8_t destPos)
{
    // sort out up/down direction and issue command
    // 100ish is down
    // 0ish is up
    uiRequestedPosition = destPos;
    int8_t delta = destPos - uiEstimatedPosition;
    if (delta > 0 || destPos == 100)
    {
        cmd_down();
        Serial.printf("%d steps to go down\n", delta);
        counter_stepsToGo.set(delta);
    }
    if (delta < 0 || destPos == 0)
    {
        cmd_up();
        Serial.printf("%d steps to go up\n", -delta);
        counter_stepsToGo.set(-delta);
    }
    return *this;
}

/* Public event methods
 *
 */
SknAtmDoor& SknAtmDoor::cmd_down() {
	counter_stepsToGo.set(ATM_COUNTER_OFF);
	trigger(EVT_CMD_DOWN);
	return *this;
}

SknAtmDoor& SknAtmDoor::cmd_stop() {
	counter_stepsToGo.set(ATM_COUNTER_OFF);
	trigger(EVT_CMD_STOP);
	return *this;
}

SknAtmDoor& SknAtmDoor::cmd_up() {
	counter_stepsToGo.set(ATM_COUNTER_OFF);
	trigger(EVT_CMD_UP);
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
    "DOOR\0EVT_STEP\0EVT_CMD_DOWN\0EVT_CMD_STOP\0EVT_CMD_UP\0EVT_POS_REACHED\0ELSE\0STOPPED\0MOVING_UP\0UP\0MOVING_DOWN\0DOWN" );
  return *this;
}
