#define SERIAL_BAUD_RATE 115200
#define MUX_SEL_0        26
#define MUX_SEL_1        27
#define MUX_SEL_2        14
#define MUX_SEL_3        12
#define MUX_INPUT_A      36
#define MUX_INPUT_B      39

int pin_mask;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  pinMode(MUX_SEL_0, OUTPUT);
  pinMode(MUX_SEL_1, OUTPUT);
  pinMode(MUX_SEL_2, OUTPUT);
  pinMode(MUX_SEL_3, OUTPUT);
  pin_mask = 0;
}

void loop() {
  if (pin_mask >= 0x10) {
    pin_mask = 0;
  }

  digitalWrite(MUX_SEL_0, pin_mask & 0b00001);
  digitalWrite(MUX_SEL_1, pin_mask & 0b00010);
  digitalWrite(MUX_SEL_2, pin_mask & 0b00100);
  digitalWrite(MUX_SEL_3, pin_mask & 0b01000);
  delayMicroseconds(2);

  int value_a = analogRead(MUX_INPUT_A);
  int value_b = analogRead(MUX_INPUT_B);

  Serial.printf("C%d: %d, C%d: %d\n", pin_mask, value_a, pin_mask + 0x10, value_b);

  pin_mask++;
  delay(1000);
}
