#pragma once

#include "Config.h"

#include "Calibration.hpp"
#include "DriverProtocol.hpp"
#include "ForceFeedback.hpp"

#if ENABLE_MEDIAN_FILTER
  #include <RunningMedian.h>
#endif

class Finger : public EncodedInput, public Calibrated, public ServoForceFeedback {
 public:
  Finger(EncodedInput::Type enc_type, DecodedOuput::Type dec_type, int pin, int servo_pin) :
    ServoForceFeedback(dec_type, servo_pin),
    type(enc_type), pin(pin), value(0),
    median(MEDIAN_SAMPLES), calibrator(0, ANALOG_MAX, CLAMP_ANALOG_MAP) {}

  void readInput() override {
    // Read the latest value.
    int new_value = analogRead(pin);

    // Apply configured modifiers.
    #if FLIP_POTS
      new_value = ANALOG_MAX - new_value;
    #endif

    #if ENABLE_MEDIAN_FILTER
      median.add(new_value);
      new_value = median.getMedian();
    #endif

    #if CLAMP_FLEXION
      new_value = new_value > CLAMP_MAX ? CLAMP_MAX : new_value;
      new_value = new_value < CLAMP_MIN ? CLAMP_MIN : new_value;
    #endif

    // Update the calibration
    if (calibrate) {
      calibrator.update(new_value);
    }

    // set the value to the calibrated value.
    value = calibrator.calibrate(new_value, 0, ANALOG_MAX);
  }

  inline int getEncodedSize() const override {
    // Encode string size = AXXXX
    return 5;
  }

  int encode(char* output) const override {
    return snprintf(output, getEncodedSize(), "%c%d", type, value);
  }

  void resetCalibration() override {
    calibrator.reset();
  }

  virtual int flexionValue() const {
    return value;
  }

 protected:
  EncodedInput::Type type;
  int pin;
  int value;

  #if ENABLE_MEDIAN_FILTER
    RunningMedian median;
  #else
    int median;
  #endif

  MinMaxCalibrator<int> calibrator;
};

class SplayFinger : public Finger {
 public:
  SplayFinger(EncodedInput::Type enc_type, DecodedOuput::Type dec_type, int pin, int splay_pin, int servo_pin) :
    Finger(enc_type, dec_type, pin, servo_pin), splay_pin(splay_pin), splay_value(0) {}

  void readInput() override {
    Finger::readInput();
    splay_value = analogRead(splay_pin);
  }

  inline int getEncodedSize() const override {
    // Encoded string size = AXXXX(AB)XXXX
    return 13;
  }

  int encode(char* output) const override {
    return snprintf(output, getEncodedSize(), "%c%d(%cB)%d", type, value, type, splay_value);
  }

  virtual int splayValue() const {
    return splay_value;
  }

 protected:
  int splay_pin;
  int splay_value;
};