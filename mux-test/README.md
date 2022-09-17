# Multiplexer Test Utilty
This is a small tool that can be helpful to find issues with wiring of the glove. It displays the raw values read for each channel of the atatched multiplexers.

## Usage
Open mux-test.ino

Set the pin definatino to match what you are planning to use in the FW:
```
#define MUX_SEL_0        26
#define MUX_SEL_1        27
#define MUX_SEL_2        14
#define MUX_SEL_3        12
#define MUX_INPUT_A      36
#define MUX_INPUT_B      39
```

Flash mux-test.ino to your ESP32 as you would with the normal FW.

Open the Serial Monitor in Arduino IDE.

In the Serial Monitor you should now see:
```
C0: 1806, C16: 109
C1: 946, C17: 25
C2: 362, C18: 0
C3: 58, C19: 0
C4: 0, C20: 0
C5: 4095, C21: 542
C6: 1646, C22: 71`
C7: 858, C23: 11
C8: 4095, C24: 529
C9: 4095, C25: 475
C10: 0, C26: 0
C11: 4095, C27: 533
C12: 1379, C28: 34
C13: 4095, C29: 528
C14: 1613, C30: 64
C15: 4095, C31: 539
```

The tool will continue to cycle through the channels, so it is recommended to set your potentiometers to random values and let it cycle through multiple times to verify everything is working.