// Program used to test the USB Joystick object on the 
// Arduino Leonardo or Arduino Micro.
 
// CONFIG
#define MAX_SWITCHES 10 // the number of switches
byte switch_pin[MAX_SWITCHES] = {15, 14, 16, 10, 1,0,9,8,7,6}; // digital input pins
#define MAX_ANALOG 4 // the number of analog inputs
byte analog_pin[MAX_ANALOG] = {A0,A1,A2,A3}; // analog input pins X,Y,Z,THROT
#define HAT_SWITCH_COUNT 1 // number of HAT buttons
#define HAT_SWITCHES 4 // the number of switches per HAT button
byte hat_pin[MAX_SWITCHES] = {2,3,4,5}; // digital input pins
// END CONFIG

// DECLARATIONS
#include "Joystick.h"
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, MAX_SWITCHES,
  HAT_SWITCH_COUNT,
  true, true, false,
  false, false, false,
  true, true, false, false, false);
byte switch_state[MAX_SWITCHES];
byte switch_state_old[MAX_SWITCHES];
byte hat_switch_state[HAT_SWITCHES];
int analog_value[MAX_ANALOG]; // +1 for the rotary encoder value
// END DECLARATIONS

// SETUP
void setup() {
  for (byte i=0; i<MAX_SWITCHES; i++) pinMode(switch_pin[i],INPUT_PULLUP);
  for (byte i=0; i<HAT_SWITCHES; i++) pinMode(hat_pin[i],INPUT_PULLUP);
  pinMode(13,OUTPUT); // on board LED
  digitalWrite(13,0); 
  Joystick.setXAxisRange(-511, 511);
  Joystick.setYAxisRange(-511, 511);
  Joystick.setRudderRange(-511, 511);
  Joystick.setThrottleRange(0, 1023);

  // Initialize Joystick Library
  Joystick.begin();
} // END SETUP

// LOOP
void loop() {
  for (byte i=0; i<MAX_SWITCHES; i++) { // read the switches
    switch_state[i] = !digitalRead(switch_pin[i]);
    if (switch_state[i] != switch_state_old[i]) { // debounced button has changed state
      // this code is executed once after change of state
      digitalWrite(13,switch_state[i]);
      if (switch_state[i]) Joystick.pressButton(i); else Joystick.releaseButton(i);
      switch_state_old[i] = switch_state[i]; // store new state such that the above gets done only once
    }
  } //END read the switches

  bool hatValueChanged = false;
  for (int index = 0; index < HAT_SWITCHES; index++)
  {
    int currentButtonState = !digitalRead(hat_pin[index]);
    if (currentButtonState != hat_switch_state[index])
    {
      hatValueChanged = true;
      hat_switch_state[index] = currentButtonState;
    }
  } //END read HAT switches

  if (hatValueChanged) {
    if ((hat_switch_state[0] == 0)
      && (hat_switch_state[1] == 0)
      && (hat_switch_state[2] == 0)
      && (hat_switch_state[3] == 0)) {
        Joystick.setHatSwitch(0, -1);
    } else 
    if (hat_switch_state[0] == 1 && hat_switch_state[1] == 1) {
      Joystick.setHatSwitch(0, 45);
    } else 
    if (hat_switch_state[0] == 1 && hat_switch_state[3] == 1) {
      Joystick.setHatSwitch(0, 315);
    } else
    if (hat_switch_state[0] == 1) {
      Joystick.setHatSwitch(0, 0);
    } else
    if (hat_switch_state[1] == 1 && hat_switch_state[2] == 1) {
      Joystick.setHatSwitch(0, 135);
    } else 
    if (hat_switch_state[1] == 1) {
      Joystick.setHatSwitch(0, 90);
    } else
    if (hat_switch_state[2] == 1 && hat_switch_state[3] == 1) {
      Joystick.setHatSwitch(0, 225);
    } else 
    if (hat_switch_state[2] == 1) {
      Joystick.setHatSwitch(0, 180);
    } else 
    if (hat_switch_state[3] == 1) {
      Joystick.setHatSwitch(0, 270);
    }
  }

  for (byte i=0; i<MAX_ANALOG; i++) { // read analog inputs
    analog_value[i] = analogRead(analog_pin[i]);
    if (analog_value[i] < 256) analog_value[i] = analog_value[i] * 1.5;
    else if (analog_value[i] < 768) analog_value[i] = 256 + analog_value[i] / 2;
    else analog_value[i] = 640 + (analog_value[i] - 768) * 1.5;
    switch(i) {
      case 0:
        Serial.println(analogRead(analog_pin[i]));
        Joystick.setXAxis(511 - analog_value[0]);
      break;
      case 1:
        Joystick.setYAxis(511 - analog_value[1]);
      break;
      case 2:
        Joystick.setRudder(511 - analog_value[2]);
      break;
      case 3:
        Joystick.setThrottle(analog_value[3]);
      break;
    }
  }
  Joystick.sendState();
  delay(10);
} // END LOOP
