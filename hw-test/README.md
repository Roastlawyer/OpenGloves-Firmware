# Harware Test Utilty
This is a small tool that can be helpful to find issues with wiring of the glove. It displays the raw values read for a selectable pin so you can see if your sensors are working as expected.

## Usage
Open the Serial Monitor in Arduino IDE.

Flash hw-test.ino to your ESP32 as you would with the normal FW.

In the Serial Monitor you should now see:
```Select the GPIO pin you want to test [1-40]: ```

Type the pin you want to monitor in and hit `[return]`

In the Serial Monitor your should now see:
```Pin 36 selected. Send any character to start monitoring. Send any character to stop.```

Verify the pin selected matches what you input.

Press `[return]` in the input box to start monitoring.

In the Serial Monitor you should now see the pin data streaming:
```
Pin 36: 1870
Pin 36: 1869
Pin 36: 1872
Pin 36: 1872
Pin 36: 1873
Pin 36: 1872
Pin 36: 1873
Pin 36: 1872
Pin 36: 1872
...
```

Press `[return]` again in the input box to stop monitoring the current pin. You can now enter a new pin to test.
