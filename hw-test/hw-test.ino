enum State {
  SELECT_PIN,
  WAIT_TO_START,
  MONITOR_PIN,
};

State state = State::SELECT_PIN;
int pin = -1;

void setup() {
  Serial.begin(115200);
  delay(300);
}

void loop() {
  switch (state){
    case SELECT_PIN: {
      Serial.printf("Select the GPIO pin you want to test [1-40]: \n");
      // Wait for user input
      while (!Serial.available()) {};
      // Parse the int
      pin = Serial.parseInt();
      // Read the newline char
      Serial.readString();
      state = WAIT_TO_START;
      break;
    }
    case WAIT_TO_START: {
      Serial.printf("Pin %d selected. Send any character to start monitoring. Send any character to stop.", pin);
      // Wait for user input
      while (!Serial.available()) {};
      Serial.readString();
      state = MONITOR_PIN;
      break;
    }
    case MONITOR_PIN: {
      if (Serial.available()) {
        Serial.readString();
        state = SELECT_PIN;
      } else {
        Serial.printf("Pin %d: %d\n", pin, analogRead(pin));
        // Small delay to not flood the Serial Monitor.
        delay(10);
      }
      break;
    }
  };
}
