/**
 * Automaton Node for SknAtmDoor
 *
 */

#pragma once

#include <Automaton.h>
#include "SknLoxRanger.hpp"

class SknAtmDoor : public Machine {

public:
    SknAtmDoor(uint8_t relayPin, SknLoxRanger &lox);


    SknAtmDoor& begin(void);
    SknAtmDoor& trace(Stream &stream);
    SknAtmDoor& trigger(int event);

    SknAtmDoor& onChange(Machine &machine, int event = 0);
    SknAtmDoor& onChange(atm_cb_push_t callback, int idx = 0);
    SknAtmDoor& onChange(atm_cb_lambda_t callback, int idx = 0);

    SknAtmDoor& onPos(Machine &machine, int event = 0);
    SknAtmDoor& onPos(atm_cb_push_t callback, int idx = 0);
    SknAtmDoor& onPos(atm_cb_lambda_t callback, int idx = 0);

    /* External Action Helpers */
    SknAtmDoor& cmd_auto_learn_up(void);        // Learn door boundaries
    SknAtmDoor& cmd_auto_learn_down(void);        // Learn door boundaries
    SknAtmDoor& cmd_down(void);                 // triggers Door MOVING_DOWN event
    SknAtmDoor& cmd_stop(void);                 // triggers Door STOPPED event
    SknAtmDoor& cmd_up(void);                   // triggers Door MOVING_UP event
    SknAtmDoor& cmd_pos(uint8_t destPos);       // triggers Door MOVING_POS event
    
    /* utilities */
    SknAtmDoor& setDoorPosition_cb(uint8_t currentPosition);
    const char * mapstate(int state);
    int state(void);

    enum eStates { STOPPED, MOVING_UP, UP, MOVING_DOWN, DOWN, MOVING_POS, LEARN_UP, LEARN_DOWN }; // STATES
    enum eEvents { EVT_DOWN, EVT_STOP, EVT_UP, EVT_POS, EVT_POS_REACHED, EVT_TIMER, EVT_LEARN_UP, EVT_LEARN_DOWN, ELSE }; // EVENTS

private:
    enum eActions { ENT_STOPPED, ENT_MOVING_UP, ENT_UP, ENT_MOVING_DOWN, ENT_DOWN, ENT_POS, ENT_LEARN, LP_POS, LP_LEARN }; // ACTIONS
    enum eConnectors { ON_CHANGE, ON_POS, ON_LEARN, CONN_MAX }; // CONNECTORS

    atm_timer_millis autoLearnTime;

    /*
     * Implement external effects */
    SknAtmDoor& relayPause(unsigned int _ms); // implements a while(millis) loop since delay doesn't work well
    SknAtmDoor& relayStart();             // implements normal click (384ms) to start door movement
    SknAtmDoor& relayStop();              // implements quick (64ms) click to stop door
    SknAtmDoor& relayChangeDirection();   // quick to stop door, then normal click to start it
    void doorStartAutoLearn();            // Enable door range position
    void doorMove();                      // clicks door relay and turns on ranger
    void doorStop();                      // clicks door relay and turns off ranger
    void doorHalt();                      // turn off ranger operations only
    void doorChangeDirection();           // changes door active direction by quick stop and normal click of door relay

    int event(int id);                    // event handler driven by calulated events or external events
    void action(int id);                  // Action Handler driven by state changes

    atm_connector connectors[CONN_MAX];   // publishing interface for OnPush() and onPos() subscribers
    uint8_t uiEstimatedPosition;          // actual tranlated positon from VL53L1x
    uint8_t uiRequestedPosition;          // from/for cmd_Pos(value) requested by interface
    uint8_t uiRelayPin;                   // pin for locally managed door relay
    uint8_t uiLastEstimatedPosition=0;    // used to signal a position change by action()

    #define MAX_SAMPLES 5
    bool bChangeDirectionEnabled=false;    // indicates cmd_Pos() is active
    int iChangeDirectionCounter=0;         // counter to enure change direction flag has happened x time in a row
    int iSamples = 0;                      // sample counter used by setDoorPosition
    int iSampleCount = 0;                  // number of position samples received
    int eExpectedPosDirection = STOPPED;   // Expected Direction of a POS Move
    int eRequestedDirection = STOPPED;     // Requested cmd state by incoming events
    unsigned int iaDirection[MAX_SAMPLES+3]; // used to determine current direction based on last x values a[first] > a[last]


    SknLoxRanger& ranger;                   // external time of flight distance measuring device VL53L1x
};

