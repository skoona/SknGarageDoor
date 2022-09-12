/**
 * @file SknAtmDigital.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <Automaton.h>

// Digital pin with a minimum duration in ms
// On detection another machine is messaged or a callback is fired

class SknAtmDigital : public Machine {
 public:
  enum { IDLE, WAITH, VHIGH, WAITL, VLOW };     // STATES
  enum { EVT_TIMER, EVT_HIGH, EVT_LOW, ELSE };  // EVENTS

  SknAtmDigital( void );
  SknAtmDigital& begin( int pin, int debounce = 20, bool activeLow = false, bool pullUp = false );
  int state( void );
  SknAtmDigital& onChange( bool status, atm_cb_push_t callback, int idx = 0 );
  SknAtmDigital& onChange( bool status, Machine& machine, int event = 0 );
  SknAtmDigital& onChange( atm_cb_push_t callback, int idx = 0 );
  SknAtmDigital& onChange( Machine& machine, int event = 0 );
  SknAtmDigital& onChange( bool status, atm_cb_lambda_t callback, int idx = 0); 
  SknAtmDigital& led( int led, bool activeLow = false );
  SknAtmDigital& trace( Stream& stream );

 private:
  enum { ENT_HIGH, ENT_LOW };                             // ACTIONS
  enum { ON_CHANGE_FALSE, ON_CHANGE_TRUE, _CONN_SIZE_ };  // CONNECTORS
  short pin;
  atm_timer_millis timer;
  bool activeLow;
  atm_connector connection[_CONN_SIZE_];
  int8_t indicator;
  bool indicatorActiveLow;

  int event( int id );
  void action( int id );
};
