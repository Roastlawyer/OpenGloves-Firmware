#pragma once

#include "Config.h"

#include "Calibration.hpp"
#include "DriverProtocol.hpp"
#include "Pin.hpp"

// Base finger class with externalized features.
class Finger : public EncodedInput, public Calibrated {
 public:
  virtual int curlValue() const = 0;
  virtual int splayValue() const = 0;
 protected:
  Finger(EncodedInput::Type type, bool invert_curl, bool invert_splay) : type(type), invert_curl(invert_curl), invert_splay(invert_splay) {}

  EncodedInput::Type type;
  bool invert_curl;
  bool invert_splay;
};

#define ConstructorArgs EncodedInput::Type type, bool invert_curl, bool invert_splay, MultiSourcePin* k0,  MultiSourcePin* k1,  MultiSourcePin* k2,  MultiSourcePin* splay
#define all_args type, invert_curl, invert_splay, k0, k1, k2, splay

// Declare unspecialized type, but don't define it so any unspecialized version won't compile (eg knuckle count 4).
template<bool enable_splay, size_t knuckle_count,
         size_t knuckle_offset=EncodedInput::KnuckleFingerOffset,
         typename CurlCalibrator=CALIBRATION_CURL, typename SplayCalibrator=CALIBRATION_SPLAY>
class ConfigurableFinger;

/*
 * ConfigurableFinger: 1 knuckle, no splay
 */
template<size_t _, typename CurlCalibrator, typename SplayCalibrator>
class ConfigurableFinger<false, 1, _, CurlCalibrator, SplayCalibrator> : public Finger {
 public:
  ConfigurableFinger(ConstructorArgs) : Finger(type, invert_curl, invert_splay), pin(k0), value(0) {}

  void readInput() override {
    // Read the latest value.
    int new_value = pin->analogRead();

    // Apply configured modifiers.
    if (invert_curl) {
      new_value = ANALOG_MAX - new_value;
    }

    // Update the calibration
    if (calibrate) {
      calibrator.update(new_value);
    }

    // set the value to the calibrated value.
    value = calibrator.calibrate(new_value);
  }

  inline int getEncodedSize() const override {
    return EncodedInput::CurlSize;
  }

  int encode(char* output) const override {
    return snprintf(output, getEncodedSize(), EncodedInput::CurlFormat, type, value);
  }

  void resetCalibration() override {
    calibrator.reset();
  }

  int curlValue() const override {
    return value;
  }

  int splayValue() const override {
    // This finger type doesn't have splay so just return the center for anyone that asks.
    return ANALOG_MAX / 2;
  }

  // Allow others access to the finger's calibrator so they can
  // map other values on this range.
  int mapOntoCalibratedRange(int input, int min, int max) const {
    return calibrator.calibrate(input);
  }

 protected:
  MultiSourcePin* pin;
  int value;
  CurlCalibrator calibrator;
};

/*
 * ConfigurableFinger: 2 knuckles, no splay
 */
template<size_t knuckle_offset, typename CurlCalibrator, typename SplayCalibrator>
class ConfigurableFinger<false, 2, knuckle_offset, CurlCalibrator, SplayCalibrator> : public Finger {
 public:
  ConfigurableFinger(ConstructorArgs) : Finger(type, invert_curl, invert_splay), pins{k0, k1},
                                        values{0, 0, 0} {}

  void readInput() override {
    // Read from the two pins that we have.
    for (size_t i = 0; i < 2; i++) {
      // Read the latest value.
      int new_value = pins[i]->analogRead();

      // Apply configured modifiers.
      if (invert_curl) {
        new_value = ANALOG_MAX - new_value;
      }

      // Update the calibration
      if (calibrate) {
        calibrators[i].update(new_value);
      }

      // Set the value to the calibrated value.
      values[i] = calibrators[i].calibrate(new_value);
    }

    // The third knuckle is based on the second knuckle.
    constexpr int min = ANALOG_MAX * KNUCKLE_DEPENDENCY_START;
    constexpr int max = ANALOG_MAX * KNUCKLE_DEPENDENCY_END;
    values[2] = accurateMap(constrain(values[1], min, max), min, max, 0, ANALOG_MAX);
  }

  inline int getEncodedSize() const override {
    return EncodedInput::KnuckleSize + EncodedInput::KnuckleSize + EncodedInput::KnuckleSize - 2;
  }

  int encode(char* output) const override {
    int offset = 0;
    for (size_t i = 0; i < 3; i++) {
      offset += snprintf(output + offset, EncodedInput::KnuckleSize,
                         EncodedInput::knuckleFormat(i + knuckle_offset), this->type, values[i]);
    }
    return offset;
  }

  void resetCalibration() override {
    for (size_t i = 0; i < 2; i++) {
      calibrators[i].reset();
    }
  }

  int curlValue() const override {
    // Take the average of all knuckles
    return (values[0] + values[1] + values[2]) / 3.0f;
  }

  int splayValue() const override {
    // This finger type doesn't have splay so just return the center for anyone that asks.
    return ANALOG_MAX / 2;
  }

  // Allow others access to the finger's calibrator so they can
  // map other values on this range.
  int mapOntoCalibratedRange(int input, int min, int max) const {
    // TODO: figure out how to make this work.
    return 0;
    //return calibrator.calibrate(input, min, max);
  }

 protected:
  MultiSourcePin*  pins[2];
  int values[3];
  CurlCalibrator calibrators[2];
};

/*
 * ConfigurableFinger: 3 knuckle, no splay
 */
template<size_t knuckle_offset, typename CurlCalibrator, typename SplayCalibrator>
class ConfigurableFinger<false, 3, knuckle_offset, CurlCalibrator, SplayCalibrator> : public Finger {
   public:
  ConfigurableFinger(ConstructorArgs) : Finger(type, invert_curl, invert_splay), pins{k0, k1, k2},
                                        values{0, 0, 0} {}

  void readInput() override {
    // Read from the three pins that we have.
    for (size_t i = 0; i < 3; i++) {
      // Read the latest value.
      int new_value = pins[i]->analogRead();

      // Apply configured modifiers.
      if (invert_curl) {
        new_value = ANALOG_MAX - new_value;
      }

      // Update the calibration
      if (calibrate) {
        calibrators[i].update(new_value);
      }

      // set the value to the calibrated value.
      values[i] = calibrators[i].calibrate(new_value);
    }
  }

  inline int getEncodedSize() const override {
    return EncodedInput::KnuckleSize + EncodedInput::KnuckleSize + EncodedInput::KnuckleSize - 2;
  }

  int encode(char* output) const override {
    int offset = 0;
    for (size_t i = 0; i < 3; i++) {
      offset += snprintf(output + offset, EncodedInput::KnuckleSize,
                         EncodedInput::knuckleFormat(i + knuckle_offset), this->type, values[i]);
    }
    return offset;
  }

  void resetCalibration() override {
    for (size_t i = 0; i < 3; i++) {
      calibrators[i].reset();
    }
  }

  int curlValue() const override {
    // Take the average of all knuckles
    return (values[0] + values[1] + values[2]) / 3.0f;
  }

  int splayValue() const override {
    // This finger type doesn't have splay so just return the center for anyone that asks.
    return ANALOG_MAX / 2;
  }

  // Allow others access to the finger's calibrator so they can
  // map other values on this range.
  int mapOntoCalibratedRange(int input, int min, int max) const {
    // TODO: figure out how to make this work.
    return input;
    //return calibrator.calibrate(input, min, max);
  }

 protected:
  MultiSourcePin*  pins[3];
  int values[3];
  CurlCalibrator calibrators[3];
};

/*
 * SplaySupport: adds splay to any Finger type.
 * If you are adding a custom finger, see the partially specialized class that adds splay support
 * to all ConfigurableFingers and use that as an example.
 */
template<typename SplayCalibrator, typename BaseFinger>
class SplaySupport : public BaseFinger {
 public:
  SplaySupport(ConstructorArgs) : BaseFinger(all_args), splay_pin(splay), splay_value(0) {}

  void readInput() override {
    BaseFinger::readInput();
    int new_splay_value = splay_pin->analogRead();
    // Update the calibration
    if (this->calibrate) {
      splay_calibrator.update(new_splay_value);
    }

    if (this->invert_splay) {
      new_splay_value = ANALOG_MAX - new_splay_value;
    }

    // Set the value to the calibrated value.
    splay_value = splay_calibrator.calibrate(new_splay_value);
  }

  inline int getEncodedSize() const override {
    return BaseFinger::getEncodedSize() + EncodedInput::SplaySize - 1;
  }

  int encode(char* output) const override {
    int offset = BaseFinger::encode(output);
    offset += snprintf(output + offset, EncodedInput::SplaySize, EncodedInput::SplayFormat,
                       this->type, splay_value);
    return offset;
  }

  void resetCalibration() override {
    BaseFinger::resetCalibration();
    splay_calibrator.reset();
  }

  virtual int splayValue() const {
    return splay_value;
  }

 protected:
  MultiSourcePin* splay_pin;
  int splay_value;
  SplayCalibrator splay_calibrator;
};

/*
 * This is a partially specialized class that allows ANY knuckle count, but always has splay enabled.
 */
template<size_t knuckle_count, size_t knuckle_offset, typename CurlCalibrator,
         typename SplayCalibrator>
class ConfigurableFinger<true, knuckle_count, knuckle_offset, CurlCalibrator, SplayCalibrator> :
  public SplaySupport<SplayCalibrator, ConfigurableFinger<false, knuckle_count, knuckle_offset,
                                                          CurlCalibrator, SplayCalibrator>> {
 public:
  ConfigurableFinger(ConstructorArgs) : SplaySupport<SplayCalibrator,
                                                     ConfigurableFinger<false, knuckle_count,
                                                                        knuckle_offset,
                                                                        CurlCalibrator,
                                                                        SplayCalibrator>
                                                    >(all_args) {}
};
