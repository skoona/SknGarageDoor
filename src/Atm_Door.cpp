/**
 * Automaton Node for Atm_Door
 *
 */
#include "Atm_Door.hpp"


Atm_Door::Atm_Door( SknLoxRanger& lox) :
    Machine(),
    estimatedPosition(100),
    ranger(lox),
    on(false)
    {};


