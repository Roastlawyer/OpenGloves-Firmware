#pragma once

#include "Config.h"
#include "ICommunication.hpp"
#include <WiFi.h>

#define WIFI_READ_BUFFER_LEN 100
class WIFISerialCommunication : public ICommunication {
 private:
  WiFiServer m_server{WIFI_SERIAL_PORT};
  WiFiClient m_client;

 public:
  WIFISerialCommunication() {}

  void start() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SERIAL_SSID, WIFI_SERIAL_PASSWORD);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.printf("WiFI connection failed!\n");
      return;
    }

    Serial.begin(115200);
    Serial.println("Your board is now connected to: ");
    Serial.println(WiFi.localIP());
    m_server.begin();
  }

  bool isOpen() {
    // Set the client.
    m_client = m_server.available();

    // Check the status of the client.
    return m_client && m_client.connected();
  }

  bool hasData() {
    // Only call this if isOpen() returns true.
    return m_client.available() > 0;
  }

  void output(char* data) {
    // Only call this if isOpen() returns true.
    m_client.write(data);
    m_client.flush();
  }

  bool readData(char* input, size_t buffer_size) {
    // Only call this if isOpen() returns true.
    size_t size = m_client.readBytesUntil('\n', input, buffer_size);
    input[size] = '\0';
    return size > 0;
  }
};
