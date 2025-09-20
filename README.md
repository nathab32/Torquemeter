
# Torquemeter

## About

Static torquemeter designed for rubber powered flying devices (planes, helicopters). The design uses a lever arm, load cell, instrumentation amplifier, and 24-bit ADC. The code uses my own button, NAU7802, and moving average libraries. The control of the display is handled by Adafruit's SSD1306 and GFX libraries, while a low pass filter is copied from curiores's ArduinoTutorials repository (found [here](https://github.com/curiores/ArduinoTutorials/blob/main/BasicFilters/ArduinoImplementations/LowPass/LowPass2.0/LowPass2.0.ino)).

### Features

- Maximum torque of 230gf-cm, or about 3.2 oz-in.
- Easy to switch to different units (oz-in, gf-cm, in-lb, mN-m)
- Tare feature to compare torques
- Program can be easily calibrated and reflashed
- Overload warning
- Stopwatch to manage winding of rubber bands
- More features planned in the future (Battery power, serial output, logging to spreadsheet, 3D printed enclosure)

## Parts

- Arduino Nano w/ USB-Type C (or Uno if not inside enclosure)
- 100g load cell (Mine is an unspecified one from Amazon, but is very similar to the [TAL221](https://cdn.sparkfun.com/assets/9/9/a/f/3/TAL221.pdf))
- EL8172 instrumentation amplifier
- NAU7802 ADC
- I2C 128x64 display
- 3x SPST momentary push buttons
- Resistors, capacitors (see schematic for specifics)
- 7mm OD, 3mm ID, 3mm thick ball bearing
- Piano wire for making hook
- 100g calibration weight

## Schematic

I haven't made the PCB yet, but once that is done, I will upload the full KiCAD files to the repository.

![Schematic image](https://raw.githubusercontent.com/nathab32/Torquemeter/refs/heads/main/images/schematic.png)

## 3D Prints

The enclosure has not been tested, but a functional version can be made using a simple test bench for the load cell and lever arm. Print `Calibration.step`, `LeverArm.step`, and `LoadCellTester.step` inside the `3DFiles` folder.

1. Secure the load cell using 2x M3x12 screws, and 2x M3 nuts on the bottom. My load cell has threads on the wire side, which make the threads on the nuts misaligned and the assembly is loose. You may consider drilling out the threads if you have them so the load cell is more secure, but it works well enough without drilling them out.
2. Push the lever arm into the ball bearing.
3. Bend the hook without the right angle as shown in `images/testBench.jpg`, and stick it through the lever arm/ball bearing assembly.
4. Bend the right angle in the piano wire so that the angle lies in one of the cutouts when the hook is facing upwards.
5. (Optional) Glue the hook in the front and back, being careful to not get glue in the bearing.

## Code

Inside `src` is the `main.cpp` file that will run on the Arduino. Upload this using PlatformIO in VSCode to start calibration.

### Calibration

1. With the calibration disk placed on the load cell, turn on the Arduino.
2. If `debug = true` inside of main, there should be 6 values being printed out on the screen. Tared voltage and mass should be close to 0.
3. Place the 100g calibration weight onto the calibration disk, and start monitoring the `calValue` line.
4. Take an estimate of the `calValue`, then set `calValue` in `main.cpp` to the estimate.
5. Take off the calibration weight and reset the Arduino.
6. Set the calibration weight down again, and the mass should now read close to 100g.

### Overload

1. Without changing the setup from the last calibration step (calibration disk tared, 100g on the platform), monitor the `Raw Voltage` value.
2. Add a tiny bit of extra weight (w/ paper clips, finger, etc.), until the mass reaches your desired overload value. Keep in mind the safe overload of the TAL221 is 150g (I chose 105g).
3. When the mass reaches your chosen overload value, take note of the `Raw Voltage` value.
4. In `main.cpp`, set `overloadVoltage` to the voltage from the previous step.
5. The torque should now say `OVERLOAD` if the voltage (tared or untared) is detected to be over the set value.

### Buttons

The three buttons' functionality is summarized below. The pin, debounce time, and hold time can be customized by modifying the constructors in `main.cpp`.

| Button | Pin | Press        | Hold                |
|--------|-----|--------------|---------------------|
| 1      | 2   | switch units | N/A                 |
| 2      | 4   | tare         | switch debug on/off |
| 3      | 7   | start timer  | N/A                 |
