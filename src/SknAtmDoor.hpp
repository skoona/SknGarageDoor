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
    SknAtmDoor& cmd_down(void);
    SknAtmDoor& cmd_stop(void);
    SknAtmDoor& cmd_up(void);
    SknAtmDoor& cmd_pos(uint8_t destPos);
    
    /* utilities */
    SknAtmDoor& setDoorPosition(uint8_t currentPosition);
    const char * mapstate(int state);
    int state(void);

    enum eStates { STOPPED, MOVING_UP, UP, MOVING_DOWN, DOWN }; // STATES
    enum eEvents { EVT_STEP, EVT_DOWN, EVT_STOP, EVT_UP, EVT_POS_REACHED, ELSE }; // EVENTS

private:
    enum eActions { ENT_STOPPED, ENT_MOVING_UP, ENT_UP, ENT_MOVING_DOWN, ENT_DOWN, LP_POS }; // ACTIONS
    enum eConnectors { ON_CHANGE, ON_POS, CONN_MAX }; // CONNECTORS

    /*
     * Implement external effects */
    SknAtmDoor& relayPause(unsigned int _ms);
    SknAtmDoor& relayStart();
    SknAtmDoor& relayStop();
    SknAtmDoor& relayChangeDirection();
    void moveUp();
    void moveDn();
    void moveStp();
    void moveHalt();
    void moveChgDir();

    int event(int id);
    void action(int id);

    atm_connector connectors[CONN_MAX];
    uint8_t uiEstimatedPosition;
    uint8_t uiRequestedPosition;
    uint8_t uiRelayPin;
    bool on;
    
    #define MAX_SAMPLES 5
    int iSamples = 0;
    int iSampleCount = 0;
    bool bUpOrDown = false;  // true up,  false down
    bool bRequestedDirection = false; // true up,  false down
    int eRequestedDirection = STOPPED;
    unsigned int iaDirection[MAX_SAMPLES+3]; // verify current direction


    SknLoxRanger& ranger;
};

