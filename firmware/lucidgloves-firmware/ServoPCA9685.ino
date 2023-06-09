//only compiles if needed
#if USING_FORCE_FEEDBACK && SERVO_INTERFACE == SERVO_PCA9685
#pragma once
#include "I2C.h"
#include "PCA9685Servo.hpp"

class PCA9685ServoConnection : public IServo 
{
  private:
    bool m_isReady;
        
  public:
    PCA9685ServoConnection() {
      m_isReady = false;
    }

    bool isReady(){
      return m_isReady;
    }

    void InitServoInterface(){
      Setup_I2C();
      Servo objforsetupServo; //Needed based on the code in PCA9685Servo.cpp. A servo obj must be created to initialize the PCA board, however the normal 5 servos are created BEFORE I2C is initialized, meaning the code otherwise runs before I2C exists to transmit setup info.
      m_isReady = true;
    }
};
#endif
