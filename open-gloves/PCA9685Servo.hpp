#pragma once

#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver* pwm_singalton = nullptr;

class Servo {
  Servo() {
    if(pwm_singalton = nullptr) {
      // TODO (roastlawyer): setup pwm_singalton(pwm_board_0 in your code)

    }
  }

  void attach(int pin) {
    channel = pin;
  }

  void writeMicroseconds(int pwm_pulse_width) {
    // TODO (roastlawyer): call setPWM here using the stored channel below.
  }

  void write(float degrees) {
    // TODO (roastlawyer): convert 180 degrees to pulse width
  }

 private:
  int channel;
};