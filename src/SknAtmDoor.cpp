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
};

SknAtmDoor& SknAtmDoor::begin()
{
     // clang-format off
    const static state_t state_table[] PROGMEM = {
        /* [-- STATES --]   [------- Actions/External Outputs ----------]  [-------------------------------------------- Event/External Inputs ---------------------------------------------] */
        /*                             ON_ENTER       ON_LOOP    ON_EXIT      EVT_DOWN   EVT_STOP       EVT_UP       EVT_POS  EVT_POS_REACHED  EVT_TIMER   EVT_LEARN_UP  EVT_LEARN_DOWN  ELSE */
        /*     STOPPED */           ENT_STOPPED,           -1,        -1,  MOVING_DOWN,        -1,   MOVING_UP,   MOVING_POS,              -1,        -1,     LEARN_UP,      LEARN_DOWN,   -1,
        /*   MOVING_UP */         ENT_MOVING_UP,       LP_POS,        -1,  MOVING_DOWN,   STOPPED,          -1,           -1,              UP,        -1,           -1,              -1,   -1,
        /*          UP */                ENT_UP,           -1,        -1,  MOVING_DOWN,   STOPPED,          -1,   MOVING_POS,              -1,        -1,     LEARN_UP,      LEARN_DOWN,   -1,
        /* MOVING_DOWN */       ENT_MOVING_DOWN,       LP_POS,        -1,           -1,   STOPPED,   MOVING_UP,           -1,            DOWN,        -1,           -1,              -1,   -1,
        /*        DOWN */              ENT_DOWN,           -1,        -1,           -1,   STOPPED,   MOVING_UP,   MOVING_POS,              -1,        -1,     LEARN_UP,      LEARN_DOWN,   -1,
        /*  MOVING_POS */              ENT_POS,        LP_POS,        -1,           -1,   STOPPED,          -1,           -1,         STOPPED,        -1,           -1,              -1,   -1,
        /*    LEARN_UP */            ENT_LEARN,      LP_LEARN,        -1,           -1,   STOPPED,          -1,           -1,              -1,        STOPPED,      -1,              -1,   -1,
        /*  LEARN_DOWN */            ENT_LEARN,      LP_LEARN,        -1,           -1,   STOPPED,          -1,           -1,              -1,        STOPPED,      -1,              -1,   -1
    };
    // clang-format on
    Machine::begin(state_table, ELSE);
    autoLearnTime.set(10000);

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
    #ifndef SKN_DISABLE_RELAY
    digitalWrite(uiRelayPin, HIGH);
    relayPause(384); 
    digitalWrite(uiRelayPin, LOW); 
    #endif
    return *this;    
}
SknAtmDoor& SknAtmDoor::relayStop() {
    #ifndef SKN_DISABLE_RELAY
    digitalWrite(uiRelayPin, HIGH);
    relayPause(64); 
    digitalWrite(uiRelayPin, LOW); 
    #endif
    return *this;    
}
SknAtmDoor& SknAtmDoor::relayChangeDirection() {
    #ifndef SKN_DISABLE_RELAY
    relayStop().relayPause(10).relayStart();    
    #endif
    Serial.printf("????????? SknAtmDoor::relayChangeDirection() ep=%d, rp=%d, ePos:%s, eReq:%s, curr:%s\n", uiEstimatedPosition, uiRequestedPosition, mapstate(eExpectedPosDirection), mapstate(eRequestedDirection), mapstate(current));
    return *this;    
}

/*
 *  external effects Action helpers
*/
void SknAtmDoor::doorMove() { 
    ranger.start();
    relayStart();
}
void SknAtmDoor::doorStop() { 
    relayStop();
    ranger.stop(); 
}
void SknAtmDoor::doorHalt() { 
    ranger.stop(); 
}
void SknAtmDoor::doorStartAutoLearn() { 
    ranger.start();
}
void SknAtmDoor::doorChangeDirection() { 
    relayChangeDirection();
}

/* Add C++ code for each internally handled event (input)
 * The code must return 1 to trigger the event
 * -- ignore incoming event if we are already there
 */
int SknAtmDoor::event(int id) {
    switch (id)
    {        
    case EVT_DOWN: // dn=100      pos=50
        if((uiEstimatedPosition != 100) && (id == next_trigger)) {
            return true;
        }
        break;
    case EVT_STOP:
    case EVT_UP:   // up=0        pos=50
        if((uiEstimatedPosition != 0) && (id == next_trigger)) {
            return true;
        }
        break;
    case EVT_POS:   // up=0        pos=50
        if((uiEstimatedPosition != uiRequestedPosition) && (id == next_trigger)) {
            return true;
        }
        break;
    case EVT_POS_REACHED:
         return (id == next_trigger);
    case EVT_TIMER:
          return autoLearnTime.expired( this );
    case EVT_LEARN_UP:
         return (id == next_trigger);
    case EVT_LEARN_DOWN:
         return (id == next_trigger);
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
    // case ATM_ON_SWITCH:
    // Serial.printf("[action]ATM_ON_SWITCH id %d, nextTrigger %d, current %s, next %s, eReq:%s\n", 
    //     id, next_trigger, 
    //     mapstate(current),  mapstate(next), mapstate(eRequestedDirection));
    //     break;
    case ENT_STOPPED:
        doorHalt();
        // uiEstimatedPosition = uiRequestedPosition;
        bChangeDirectionEnabled=false;
        eExpectedPosDirection=STOPPED;
        push(connectors, ON_POS, 0, uiEstimatedPosition, uiRequestedPosition);        
        push(connectors, ON_CHANGE, 0, current, 0);
        break;
    case ENT_MOVING_UP:
        doorMove();
        push(connectors, ON_CHANGE, 0, current, 0);
        if (uiEstimatedPosition % 2 == 0) {
            push(connectors, ON_POS, 0, uiEstimatedPosition, uiRequestedPosition);        
        }
        break;
    case ENT_UP:
        doorHalt();
        push(connectors, ON_CHANGE, 0, current, 0);
        push(connectors, ON_POS, 0, uiEstimatedPosition, uiRequestedPosition);        
        break;
    case ENT_MOVING_DOWN:
        doorMove();
        push(connectors, ON_CHANGE, 0, current, 0);
        if (uiEstimatedPosition % 2 == 0) {
            push(connectors, ON_POS, 0, uiEstimatedPosition, uiRequestedPosition);        
        }
        break;
    case ENT_DOWN:
        doorHalt();
        push(connectors, ON_CHANGE, 0, current, 0);
        push(connectors, ON_POS, 0, uiEstimatedPosition, uiRequestedPosition);        
        break;
    case ENT_POS:
        doorMove();
        push(connectors, ON_CHANGE, 0, current, 0);
        if (uiEstimatedPosition % 2 == 0) {
            push(connectors, ON_POS, 0, uiEstimatedPosition, uiRequestedPosition);        
        }
        break;
    case LP_POS:
        if (uiLastEstimatedPosition != uiEstimatedPosition) {
            uiLastEstimatedPosition = uiEstimatedPosition;
            push(connectors, ON_POS, 0, uiEstimatedPosition, uiRequestedPosition);
         }
         // match within 1
        if((uiRequestedPosition==uiEstimatedPosition) || (abs(uiRequestedPosition-uiEstimatedPosition)<=1)) {
            trigger(EVT_POS_REACHED);
            push(connectors, ON_POS, 0, uiEstimatedPosition, uiRequestedPosition);
        }
        // Serial.printf("^ SknAtmDoor::Action() ep=%d, rp=%d, eReq:%s, state:%s\n", uiEstimatedPosition, uiRequestedPosition, mapstate(eRequestedDirection), mapstate(state()));
        break;
    case LP_LEARN:
        if ((cycles % 20000) == 0) {
            uiLastEstimatedPosition = uiEstimatedPosition;
            push(connectors, ON_POS, 0, uiEstimatedPosition, uiRequestedPosition);
            push(connectors, ON_CHANGE, 0, current, 0);
        }        
        break;
    case ENT_LEARN:
        doorStartAutoLearn();
        push(connectors, ON_POS, 0, uiEstimatedPosition, uiRequestedPosition);
        push(connectors, ON_CHANGE, 0, current, 0);
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
    if (destPos != uiEstimatedPosition) {
        eRequestedDirection = MOVING_POS;
        bChangeDirectionEnabled=true;
        eExpectedPosDirection = (destPos > uiEstimatedPosition ? MOVING_DOWN : MOVING_UP);
        trigger(EVT_POS);
    } else if (destPos == uiEstimatedPosition) {
        eRequestedDirection = STOPPED;
        eExpectedPosDirection = STOPPED;
        bChangeDirectionEnabled=false;
        trigger(EVT_STOP);
    }
    return *this;
}

SknAtmDoor& SknAtmDoor::cmd_auto_learn_up(void) {        // Learn door boundaries
    if(current!=DOWN) {
        uiRequestedPosition = 0;
        eRequestedDirection = LEARN_UP;
        eExpectedPosDirection = eRequestedDirection;
        bChangeDirectionEnabled=false;
        trigger(EVT_LEARN_UP);
    }
	return *this;
}
SknAtmDoor& SknAtmDoor::cmd_auto_learn_down(void) {        // Learn door boundaries
    if(current!=UP) {
        uiRequestedPosition = 100;
        eRequestedDirection = LEARN_DOWN;
        eExpectedPosDirection = eRequestedDirection;
        bChangeDirectionEnabled=false;
        trigger(EVT_LEARN_DOWN);
    }
	return *this;
}

SknAtmDoor& SknAtmDoor::cmd_down() {
    uiRequestedPosition = 100;
    eRequestedDirection = MOVING_DOWN;
    bChangeDirectionEnabled=true;
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
    bChangeDirectionEnabled=true;
	trigger(EVT_UP);
	return *this;
}

SknAtmDoor& SknAtmDoor::setDoorPosition_cb(uint8_t currentPosition) {
    uiEstimatedPosition = currentPosition;
    iSampleCount++;
    int eDir=STOPPED;

    /*
     * which way are we moving? */
    for( iSamples = 0; iSamples < MAX_SAMPLES; iSamples++) { // shift samples down
        iaDirection[iSamples] = iaDirection[iSamples+1];
    }
    iaDirection[iSamples] = currentPosition; // assign latest value to top

    if ((iSampleCount >= MAX_SAMPLES) && (current!=DOWN || current!=UP)) { 
        if (iChangeDirectionCounter<0) { iChangeDirectionCounter=0; }
        /* 
         * a > b = UP
         * a < b = DOWN 
         * a == b = STOPPED */
        if (iaDirection[0]  >  iaDirection[iSamples]) { eDir=MOVING_UP; }
        if (iaDirection[0]  <  iaDirection[iSamples]) { eDir=MOVING_DOWN; }
        if (iaDirection[0]  == iaDirection[iSamples]) { eDir=(eExpectedPosDirection>=LEARN_UP ? eExpectedPosDirection : STOPPED); }

        /*
         * If MOVING_POS allow change direction of door to reach positoning goal
         * - eExpectedPosDirection is MOVING_UP or MOVING_DOWN based on cmd_pos() bChangeDirectionEnabled decision of request
         * - Require off state to occur 5 times before executing change of direction
         * if door has been stopped too long and ranger is still running, turn off (halt) ranger
        */
        if( current==STOPPED ) {                                                       // ranger still running if door is stopped 
            iChangeDirectionCounter++;

        } else if(eDir==STOPPED && bChangeDirectionEnabled) {  // door stalled during move_pos
            iChangeDirectionCounter++;
        
        } else if( eRequestedDirection!=eDir || eExpectedPosDirection!=eDir || eDir!=STOPPED|| eExpectedPosDirection!=STOPPED ) {  // expected not matching actual direction
            iChangeDirectionCounter++;
        
        } else iChangeDirectionCounter--;

        if ( eExpectedPosDirection>= LEARN_UP) {    // ignore actions if auto learning
            iChangeDirectionCounter=0;
        }

        // handle change direction if moving_pos
        if(iChangeDirectionCounter>( 3 * MAX_SAMPLES) &&  current!=eDir  && bChangeDirectionEnabled) { // must be x in a row
            doorChangeDirection(); // door moving in wrong directions
            iChangeDirectionCounter=0;
        }

        // handle halting ranger if door has been stationary too long
        if(iChangeDirectionCounter>=( 4 * MAX_SAMPLES) && eDir==STOPPED) { // must be 2x in a row
            doorHalt();            // turn off ranger
            iChangeDirectionCounter=0;
        }

        Serial.printf("SknAtmDoor::setDoorPosition(%d:%d) Position:%d, Moving:%s, ePos:%s, sReq:%s, sCur:%s, sNext:%s, chgDir:%s, A:%d, B:%d\n", 
            iSampleCount, iChangeDirectionCounter, currentPosition, mapstate(eDir), mapstate(eExpectedPosDirection), 
            mapstate(eRequestedDirection), mapstate(current),  mapstate(next), 
            (bChangeDirectionEnabled ? "True" : "False"), iaDirection[0], iaDirection[iSamples]);
    }

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
    "DOOR\0EVT_DOWN\0EVT_STOP\0EVT_UP\0EVT_POS\0EVT_POS_REACHED\0EVT_TIMER\0EVT_LEARN_UP\0EVT_LEARN_DOWN\0ELSE\0STOPPED\0MOVING_UP\0UP\0MOVING_DOWN\0DOWN\0MOVING_POS\0LEARN_UP\0LEARN_DOWN" );
  return *this;
}
