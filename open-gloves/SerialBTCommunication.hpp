#pragma once

#include "ICommunication.hpp"
#include "BluetoothSerial.h"

class BTSerialCommunication : public ICommunication {
 private:
  bool m_isOpen;
  BluetoothSerial m_SerialBT;

 public:
  BTSerialCommunication() {
    m_isOpen = false;
  }

  bool isOpen() {
    return m_isOpen;
  }

  void start() {
    Serial.begin(SERIAL_BAUD_RATE);
    m_SerialBT.setTimeout(3);
    m_SerialBT.begin(BT_DEVICE_NAME);
    Serial.println("The device started, now you can pair it with bluetooth!");
    m_isOpen = true;
  }

  void output(char* data) {
    m_SerialBT.print(data);
    m_SerialBT.flush();
  }

  bool hasData() override {
    return m_SerialBT.available() > 0;
  }

  bool readData(char* input, size_t buffer_size) {
    size_t size = m_SerialBT.readBytesUntil('\n', input, buffer_size);
    input[size] = '\0';
    return size > 0;
  }
};
