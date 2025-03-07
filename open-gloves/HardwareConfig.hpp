#pragma once
#include "Config.h"

#include "DriverProtocol.hpp"

#include "Button.hpp"
#include "Finger.hpp"
#include "ForceFeedback.hpp"
#include "Gesture.hpp"
#include "Haptics.hpp"
#include "JoyStick.hpp"
#include "LED.hpp"
#include "Pin.hpp"

#if ENABLE_PCA_9865_SERVO
  #include <Wire.h>
#endif

StatusLED led(PIN_LED);

struct {
  void setup() {
    #if ENABLE_MULTIPLEXER
      pinMode(MUX_SEL_0, OUTPUT);
      pinMode(MUX_SEL_1, OUTPUT);
      pinMode(MUX_SEL_2, OUTPUT);
      pinMode(MUX_SEL_3, OUTPUT);
    #endif
  }
} multiplexer;

struct {
  void setup() {
    #if ENABLE_PCA_9865_SERVO
      Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
      Serial.println("I2C Initialized");
    #endif
  }
} i2c;

struct {
  void setup() {
    #if ENABLE_PCA_9865_SERVO
      Initialize_PCA9685_Board();
    #endif
  }
} PCA9685_Board;

// This button is referenced directly by the FW, so we need a pointer to it outside
// the list of buttons.
Button calibration_button(EncodedInput::Type::CALIBRATE, PIN_CALIB, INVERT_CALIB);

Button* buttons[BUTTON_COUNT] = {
  new Button(EncodedInput::Type::A_BTN, PIN_A_BTN, INVERT_A),
  new Button(EncodedInput::Type::B_BTN, PIN_B_BTN, INVERT_B),
  new Button(EncodedInput::Type::MENU, PIN_MENU_BTN, INVERT_MENU),
  &calibration_button,
  #if ENABLE_JOYSTICK
    new Button(EncodedInput::Type::JOY_BTN, PIN_JOY_BTN, INVERT_JOY),
  #endif
  #if TRIGGER_BUTTON
    new Button(EncodedInput::Type::TRIGGER, PIN_TRIGGER, INVERT_TRIGGER),
  #endif
  #if !GRAB_GESTURE
    new Button(EncodedInput::Type::GRAB, PIN_GRAB_BTN, INVERT_GRAB),
  #endif
  #if !PINCH_GESTURE
    new Button(EncodedInput::Type::PINCH, PIN_PNCH_BTN, INVERT_PINCH),
  #endif
};

#if !ENABLE_SPLAY
  #if ENABLE_THUMB
    Finger finger_thumb(EncodedInput::Type::THUMB, PIN_THUMB);
  #endif
  Finger finger_index(EncodedInput::Type::INDEX, PIN_INDEX);
  Finger finger_middle(EncodedInput::Type::MIDDLE, PIN_MIDDLE);
  Finger finger_ring(EncodedInput::Type::RING, PIN_RING);
  Finger finger_pinky(EncodedInput::Type::PINKY, PIN_PINKY);
#else
  #if ENABLE_THUMB
    SplayFinger finger_thumb(EncodedInput::Type::THUMB, PIN_THUMB, PIN_THUMB_SPLAY);
  #endif
  SplayFinger finger_index(EncodedInput::Type::INDEX, PIN_INDEX, PIN_INDEX_SPLAY);
  SplayFinger finger_middle(EncodedInput::Type::MIDDLE, PIN_MIDDLE, PIN_MIDDLE_SPLAY);
  SplayFinger finger_ring(EncodedInput::Type::RING, PIN_RING, PIN_RING_SPLAY);
  SplayFinger finger_pinky(EncodedInput::Type::PINKY, PIN_PINKY, PIN_PINKY_SPLAY);
#endif

Finger* fingers[FINGER_COUNT] = {
  #if ENABLE_THUMB
    &finger_thumb,
  #endif
  &finger_index, &finger_middle, &finger_ring, &finger_pinky
};

JoyStickAxis* joysticks[JOYSTICK_COUNT] = {
  #if ENABLE_JOYSTICK
    new JoyStickAxis(EncodedInput::Type::JOY_X, PIN_JOY_X, JOYSTICK_DEADZONE, INVERT_JOY_X),
    new JoyStickAxis(EncodedInput::Type::JOY_Y, PIN_JOY_Y, JOYSTICK_DEADZONE, INVERT_JOY_Y),
  #endif
  #if TRIGGER_AXIS
    new JoyStickAxis(EncodedInput::Type::TRIGGER_ANALOG, PIN_TRIGGER, JOYSTICK_DEADZONE, INVERT_TRIGGER)
  #endif
};

Gesture* gestures[GESTURE_COUNT] = {
  #if TRIGGER_GESTURE
    new TriggerGesture(&finger_index, ENABLE_ANALOG_TRIGGER),
  #endif
  #if GRAB_GESTURE
    new GrabGesture(&finger_index, &finger_middle, &finger_ring, &finger_pinky),
  #endif
  #if PINCH_GESTURE
    new PinchGesture(&finger_thumb, &finger_index)
  #endif
};

HapticMotor* haptics[HAPTIC_COUNT] = {
  #if ENABLE_HAPTICS
    new HapticMotor(DecodedOuput::Type::HAPTIC_FREQ,
                    DecodedOuput::Type::HAPTIC_DURATION,
                    DecodedOuput::Type::HAPTIC_AMPLITUDE, PIN_HAPTIC),
  #endif
};

ForceFeedback* force_feedbacks[FORCE_FEEDBACK_COUNT] {
  #if ENABLE_FORCE_FEEDBACK
    #if FORCE_FEEDBACK_STYLE == FORCE_FEEDBACK_STYLE_SERVO
      #if ENABLE_THUMB
        new ServoForceFeedback(DecodedOuput::Type::FFB_THUMB, &finger_thumb, PIN_THUMB_FFB, FORCE_FEEDBACK_INVERT),
      #endif
      new ServoForceFeedback(DecodedOuput::Type::FFB_INDEX, &finger_index, PIN_INDEX_FFB, FORCE_FEEDBACK_INVERT),
      new ServoForceFeedback(DecodedOuput::Type::FFB_MIDDLE, &finger_middle, PIN_MIDDLE_FFB, FORCE_FEEDBACK_INVERT),
      new ServoForceFeedback(DecodedOuput::Type::FFB_RING, &finger_ring, PIN_RING_FFB, FORCE_FEEDBACK_INVERT),
      new ServoForceFeedback(DecodedOuput::Type::FFB_PINKY, &finger_pinky, PIN_PINKY_FFB, FORCE_FEEDBACK_INVERT)
    #elif FORCE_FEEDBACK_STYLE == FORCE_FEEDBACK_STYLE_CLAMP
      #if ENABLE_THUMB
        new DigitalClampForceFeedback(DecodedOuput::Type::FFB_THUMB, &finger_thumb, PIN_THUMB_FFB),
      #endif
      new DigitalClampForceFeedback(DecodedOuput::Type::FFB_INDEX, &finger_index, PIN_INDEX_FFB),
      new DigitalClampForceFeedback(DecodedOuput::Type::FFB_MIDDLE, &finger_middle, PIN_MIDDLE_FFB),
      new DigitalClampForceFeedback(DecodedOuput::Type::FFB_RING, &finger_ring, PIN_RING_FFB),
      new DigitalClampForceFeedback(DecodedOuput::Type::FFB_PINKY, &finger_pinky, PIN_PINKY_FFB)
    #elif FORCE_FEEDBACK_STYLE == FORCE_FEEDBACK_STYLE_SERVO_CLAMP
      #if ENABLE_THUMB
        new ServoClampForceFeedback(DecodedOuput::Type::FFB_THUMB, &finger_thumb, PIN_THUMB_FFB),
      #endif
      new ServoClampForceFeedback(DecodedOuput::Type::FFB_INDEX, &finger_index, PIN_INDEX_FFB),
      new ServoClampForceFeedback(DecodedOuput::Type::FFB_MIDDLE, &finger_middle, PIN_MIDDLE_FFB),
      new ServoClampForceFeedback(DecodedOuput::Type::FFB_RING, &finger_ring, PIN_RING_FFB),
      new ServoClampForceFeedback(DecodedOuput::Type::FFB_PINKY, &finger_pinky, PIN_PINKY_FFB)
    #endif
  #endif
};
