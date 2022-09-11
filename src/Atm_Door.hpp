/**
 * Automaton Node for Atm_Door
 *
 */

#pragma once

#include <Automaton.h>
#include <SknLoxRanger.hpp>

class Atm_Door : public Machine
{

public:
    Atm_Door(SknLoxRanger &lox);

    Atm_Door &begin(void);
    Atm_Door &trace(Stream &stream);
    Atm_Door &trigger(int event);
    int state(void);
    Atm_Door &onChange(Machine &machine, int event = 0);
    Atm_Door &onChange(atm_cb_push_t callback, int idx = 0);
    Atm_Door &onChange(atm_cb_lambda_t callback, int idx = 0);
    Atm_Door &onPos(Machine &machine, int event = 0);
    Atm_Door &onPos(atm_cb_push_t callback, int idx = 0);
    Atm_Door &onPos(atm_cb_lambda_t callback, int idx = 0);

    Atm_Door &cmd_down(void);
    Atm_Door &cmd_halt(void);
    Atm_Door &cmd_up(void);
    Atm_Door &cmd_pos(uint8_t destPos);

    const char *mapstate(int state) { return mapSymbol(state + state_width - ATM_ON_EXIT, symbols); }

    enum eStates { IDLE, DOWN, MOVING_UP, UP, MOVING_DOWN, STOPPED }; // STATES
    enum eEvents { EVT_STEP, EVT_END_REACHED, EVT_CMD_DOWN, EVT_CMD_HALT, EVT_CMD_UP, EVT_POS_REACHED, ELSE }; // EVENTS

private:
    enum eActions { ENT_STOPPED, ENT_MOVING_UP, ENT_UP, ENT_MOVING_DOWN, ENT_DOWN, LP_POS }; // ACTIONS
    enum eConnectors { ON_CHANGE, ON_POS, CONN_MAX }; // CONNECTORS

    const static state_t state_table[] PROGMEM = {
        /*                             ON_ENTER       ON_LOOP    ON_EXIT     EVT_STEP    EVT_CMD_DOWN  EVT_CMD_HALT    EVT_CMD_UP  EVT_POS_REACHED  ELSE */
        /*     STOPPED */           ENT_STOPPED,           -1,        -1,          -1,    MOVING_DOWN,           -1,    MOVING_UP,              -1,   -1,
        /*   MOVING_UP */         ENT_MOVING_UP,       LP_POS,        -1,          -1,    MOVING_DOWN,      STOPPED,           -1,              UP,   -1,
        /*          UP */                ENT_UP,           -1,        -1,          -1,    MOVING_DOWN,           -1,           -1,              -1,   -1,
        /* MOVING_DOWN */       ENT_MOVING_DOWN,       LP_POS,        -1,          -1,             -1,      STOPPED,    MOVING_UP,            DOWN,   -1,
        /*        DOWN */              ENT_DOWN,           -1,        -1,          -1,             -1,           -1,    MOVING_UP,              -1,   -1,
    };

    atm_connector connectors[CONN_MAX];
    int event(int id);
    void action(int id);

    atm_counter counter_stepsToGo;

    uint8_t estimatedPosition;

    bool on;

    void moveUp()
    {
        digitalWrite(pinClose, !on);
        digitalWrite(pinOpen, on);
    }
    void moveDn()
    {
        digitalWrite(pinClose, on);
        digitalWrite(pinOpen, !on);
    }
    void moveStp()
    {
        digitalWrite(pinClose, !on);
        digitalWrite(pinOpen, !on);
    }

    SknLoxRanger &ranger;
}
