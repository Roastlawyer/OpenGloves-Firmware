#pragma once

#include "DriverProtocol.hpp"
#include "Finger.hpp"

#if ENABLE_PCA_9865_SERVO
  #include "PCA9685Servo.hpp"
#elif defined(ESP32)
  #include <ESP32Servo.h>
#else
  #include <Servo.h>
#endif

#if FORCE_FEEDBACK_SMOOTH_STEPPING
  #define SERVO_MIN !FORCE_FEEDBACK_INVERT ? MIN_PULSE_WIDTH : MAX_PULSE_WIDTH
  #define SERVO_MAX !FORCE_FEEDBACK_INVERT ? MAX_PULSE_WIDTH : MIN_PULSE_WIDTH
  #define WRITE_FUNCTION(x) writeMicroseconds(x)
#else
  #define SERVO_MIN !FORCE_FEEDBACK_INVERT ? 0.0 : 180.0
  #define SERVO_MAX !FORCE_FEEDBACK_INVERT ? 180.0 : 0.0
  #define WRITE_FUNCTION(x) write(x)
#endif

class ForceFeedback : public DecodedOuput {
 public:
  ForceFeedback(DecodedOuput::Type type, const Finger* finger) : type(type), finger(finger), limit(0) {}

  void decodeToOuput(const char* input) override {
    char* start = strchr(input, type);
    if (start != NULL) {
      limit = atoi(start + 1);
    }
  }

 protected:
  DecodedOuput::Type type;
  const Finger* finger;
  int limit;
};

// Servo based force feedback that moves the servo to the limiting
// postion.
class ServoForceFeedback : public ForceFeedback {
 public:
  ServoForceFeedback(DecodedOuput::Type type,
                     const Finger* finger,
                     int servo_pin,
                     bool invert) : ForceFeedback(type, finger), servo_pin(servo_pin), invert(invert) {}

  void setupOutput() override {
    // Initialize the servo and move it to the unrestricted base limit.
    servo.attach(servo_pin);
    servo.WRITE_FUNCTION(SERVO_MIN);
  };

  void updateOutput() override {
    servo.WRITE_FUNCTION(scale(limit));
  }

 protected:
  int scale(int input_limit) {
    #if FORCE_FEEDBACK_FINGER_SCALING
      // TODO: Does this actually scale correctly?
      // Map the Limit to the range of motion that the finger has been through.
      int out = finger->mapOntoCalibratedRange(input_limit, FORCE_FEEDBACK_MIN, FORCE_FEEDBACK_MAX);

      // Map that range onto the servo's output range.
      out = accurateMap(out, 0, ANALOG_MAX, SERVO_MIN, SERVO_MAX);

      // After mapping, make sure that we are still within the output range.
      return constrain(out, SERVO_MIN, SERVO_MAX);
    #else
      // Use the entire range of motion.
      return accurateMap(input_limit, FORCE_FEEDBACK_MIN, FORCE_FEEDBACK_MAX, SERVO_MIN, SERVO_MAX);;
    #endif
  }

  int servo_pin;
  bool invert;
  Servo servo;
};

class ClampForceFeedback : public ForceFeedback {
 public:
  ClampForceFeedback(DecodedOuput::Type type, const Finger* finger) :
    ForceFeedback(type, finger) {}

  void updateOutput() override {
    // Since the higher the limit, the less the finger should be able to move, map the finger's position onto
    // the flipped range.
    int relative_finger_position = map(finger->flexionValue(), ANALOG_MAX, 0, FORCE_FEEDBACK_MIN, FORCE_FEEDBACK_MAX);

    // Lock or unlock the clamp if the finger is at the limit.
    // Unlock the finger if the user goes too far passed. This means they have
    // overcome the brake, we release to prevent damage to the system.
    if (relative_finger_position < limit && relative_finger_position >= limit - FORCE_FEEDBACK_RELEASE) lock();
    else unlock();
  }

 protected:
  virtual void lock() = 0;
  virtual void unlock() = 0;
};

// Clamping FFB that writes the state to a digital output.
// This could be used to actuate a solenoid or some other
// binary brake.
class DigitalClampForceFeedback : public ClampForceFeedback {
 public:
  DigitalClampForceFeedback(DecodedOuput::Type type, const Finger* finger, int pin) :
    ClampForceFeedback(type, finger), pin(pin) {}

  void setupOutput() {
    pinMode(pin, OUTPUT);
    unlock();
  };

 protected:
  void lock() override {
    digitalWrite(pin, FORCE_FEEDBACK_CLAMP_LOCK);
  }

  void unlock() override {
    digitalWrite(pin, FORCE_FEEDBACK_CLAMP_UNLOCK);
  }

  int pin;
};

// Clamping FFB that uses a servo as a brake.
class ServoClampForceFeedback : public ClampForceFeedback {
 public:
  ServoClampForceFeedback(DecodedOuput::Type type, const Finger* finger, int servo_pin) :
    ClampForceFeedback(type, finger), servo_pin(servo_pin) {}

  void setupOutput() {
    // Initialize the servo and move it to "Unlocked state".
    servo.attach(servo_pin);
    unlock();
  };

 protected:
  int servo_pin;
  Servo servo;
  void lock() override {
    servo.write(FORCE_FEEDBACK_SERVO_CLAMP_LOCK);
  }

  void unlock() override {
    servo.write(FORCE_FEEDBACK_SERVO_CLAMP_UNLOCK);
  }
};
