#pragma once

#include "Config.h"

#include "Finger.hpp"
#include "DriverProtocol.hpp"

class Gesture : public EncodedInput {
 public:
  Gesture(EncodedInput::Type type) : type(type), value(false) {}

  inline int getEncodedSize() const override {
   // Encode string size = single char or '\0'
    return 1;
  }

  int encode(char* output) const override {
    if (value) output[0] = type;
    return value ? 1 : 0;
  }

  bool isPressed() {
    return value;
  }

 protected:
  const EncodedInput::Type type;
  bool value;
};

class GrabGesture : public Gesture {
 public:
  GrabGesture(const Finger* index, const Finger* middle,
              const Finger* ring, const Finger* pinky) :
    Gesture(EncodedInput::Type::GRAB),
    index(index), middle(middle), ring(ring), pinky(pinky)  {}

  // Grab gesture is pressed if the average all fingers is more than
  // halfway flexed.
  void readInput() override {
    value = (index->curlValue() + middle->curlValue() +
             ring->curlValue() + pinky->curlValue()) / 4 > ANALOG_MAX / 2;
  }

 private:
   const Finger* index;
   const Finger* middle;
   const Finger* ring;
   const Finger* pinky;
};

class TriggerGesture : public Gesture {
 public:
  TriggerGesture(const Finger* index_finger, bool analog) :
    Gesture(analog ? EncodedInput::Type::TRIGGER_ANALOG : EncodedInput::Type::TRIGGER), index_finger(index_finger), analog(analog) {}

  // Trigger gesture is pressed if the index finger is more than halfway flexed
  void readInput() override {
    if (analog) {
      value = index_finger->curlValue();
    } else {
      value = index_finger->curlValue() > ANALOG_MAX / 2;
    }
  }

  inline int getEncodedSize() const override {
    return analog ? 6 : this->Gesture::getEncodedSize();
  }

  int encode(char* output) const override {
    if (analog) {
      return snprintf(output, getEncodedSize(), "%c%d", type, value);
    } else {
      return this->Gesture::encode(output);
    }
  }

 private:
   const Finger* index_finger;
   bool analog;
};

class PinchGesture : public Gesture {
 public:
  PinchGesture(const Finger* thumb, const Finger* index_finger) :
    Gesture(EncodedInput::Type::PINCH), thumb(thumb), index_finger(index_finger) {}

  // Pinch gesture is pressed if the average flex of the thumb and index is more than
  // halfway flexed.
  void readInput() override {
    // TODO: Do we need to divide both values here?
    value = (thumb->curlValue() + index_finger->curlValue()) / 2 > ANALOG_MAX / 2;
  }

  private:
   const Finger* thumb;
   const Finger* index_finger;
};
