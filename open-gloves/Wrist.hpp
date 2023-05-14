#pragma once

#include "Config.h"

#include "DriverProtocol.hpp"

class WristAxis : public EncodedInput {
 public:
  WristAxis(EncodedInput::Type type, int pin, bool invert) :
    type(type), pin(pin), invert(invert), value(ANALOG_MAX/2) {}

  void readInput() override {
    // Read the latest value.
    int new_value = analogRead(pin);

        // Invert if required.
    if (invert) {
      new_value = ANALOG_MAX - new_value;
    }

    // Update the value.
    value = new_value;
  }

  inline int getEncodedSize() const override {
    // Encode string size = AXXXX + '\0'
    return 6;
  }

  int encode(char* output) const override {
    return snprintf(output, getEncodedSize(), "%c%d", type, value);
  }

  int getValue() const {
    return value;
  }

// private:
  
  EncodedInput::Type type;
  int pin;
  bool invert;
  int value;
};
