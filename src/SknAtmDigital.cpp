/**
 * @file SknAtmDigital.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "SknAtmDigital.hpp"


SknAtmDigital::SknAtmDigital( ) 
    : Machine()
    {};

SknAtmDigital& SknAtmDigital::begin( int pin, int debounce /* = 20 */, bool activeLow /* = false */, bool pullUp /* = false */ ) {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*              ON_ENTER    ON_LOOP      ON_EXIT  EVT_TIMER   EVT_HIGH  EVT_LOW   ELSE */
    /* IDLE    */         -1,        -1,          -1,        -1,     WAITH,      -1,    -1,
    /* WAITH   */         -1,        -1,          -1,     VHIGH,        -1,    IDLE,    -1,
    /* VHIGH   */   ENT_HIGH,        -1,          -1,        -1,        -1,   WAITL,    -1, 
    /* WAITL   */         -1,        -1,          -1,      VLOW,     VHIGH,      -1,    -1,
    /* VLOW    */    ENT_LOW,        -1,          -1,        -1,        -1,      -1,  IDLE,
  };
  // clang-format on
  Machine::begin( state_table, ELSE );
  this->pin = pin;
  this->activeLow = activeLow;
  timer.set( debounce );
  indicator = -1;
  pinMode( pin, pullUp ? INPUT_PULLUP : INPUT );
  return *this;
}

int SknAtmDigital::event( int id ) {
  switch ( id ) {
    case EVT_TIMER:
      return timer.expired( this );
    case EVT_HIGH:
      return ( !digitalRead( pin ) != !activeLow );  // XOR
    case EVT_LOW:
      return !( !digitalRead( pin ) != !activeLow );
  }
  return 0;
}

void SknAtmDigital::action( int id ) {
  switch ( id ) {
    case ENT_HIGH:
      connection[ON_CHANGE_TRUE].push( state() );
      if ( indicator > -1 ) digitalWrite( indicator, !HIGH != !indicatorActiveLow );
      return;
    case ENT_LOW:
      connection[ON_CHANGE_FALSE].push( state() );
      if ( indicator > -1 ) digitalWrite( indicator, !LOW != !indicatorActiveLow );
      return;
  }
}

int SknAtmDigital::state( void ) {
  return ( current == VHIGH || current == WAITL );
}

SknAtmDigital& SknAtmDigital::led( int led, bool activeLow /* = false */ ) {
  indicator = led;
  indicatorActiveLow = activeLow;
  pinMode( indicator, OUTPUT );
  return *this;
}

SknAtmDigital& SknAtmDigital::onChange( bool status, atm_cb_push_t callback, int idx /* = 0 */ ) {
  connection[status ? ON_CHANGE_TRUE : ON_CHANGE_FALSE].set( callback, idx );
  return *this;
}

SknAtmDigital& SknAtmDigital::onChange( bool status, Machine& machine, int event /* = 0 */ ) {
  connection[status ? ON_CHANGE_TRUE : ON_CHANGE_FALSE].set( &machine, event );
  return *this;
}

SknAtmDigital& SknAtmDigital::onChange( atm_cb_push_t callback, int idx /* = 0 */ ) {
  connection[ON_CHANGE_FALSE].set( callback, idx );
  connection[ON_CHANGE_TRUE].set( callback, idx );
  return *this;
}

SknAtmDigital& SknAtmDigital::onChange( Machine& machine, int event /* = 0 */ ) {
  connection[ON_CHANGE_FALSE].set( &machine, event );
  connection[ON_CHANGE_TRUE].set( &machine, event );
  return *this;
}
SknAtmDigital& SknAtmDigital::onChange(bool status, atm_cb_lambda_t callback, int idx) {
    connection[status ? ON_CHANGE_TRUE : ON_CHANGE_FALSE].set( callback, idx );
	return *this;
}
SknAtmDigital& SknAtmDigital::trace( Stream& stream ) {
  setTrace( &stream, atm_serial_debug::trace, "DIGITAL\0EVT_TIMER\0EVT_HIGH\0EVT_LOW\0ELSE\0IDLE\0WAITH\0VHIGH\0WAITL\0VLOW" );
  return *this;
}
