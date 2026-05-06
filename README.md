# Schottky Cable Tester (Arduino Nano)

This project tests a two-connector cable that contains a diode between its ends. The Arduino sketch in `main.ino` emulates a multimeter diode test in both polarities, compares the measured voltage drops with calibrated targets, and reports the result with two status LEDs.

- **Green LED (D5)**: the cable is good and both readings are inside the calibrated ranges.
- **Red LED (D6)**: a fault was detected, such as a missing diode, reversed diode, wrong voltage drop, or weak contact.

## How it works

The sketch drives the cable in two directions:

1. **D8 = positive**, **D9 = negative**, then measures the voltage drop as `v_ab`.
2. **D9 = positive**, **D8 = negative**, then measures the voltage drop as `v_ba`.

The readings are compared against these calibrated targets:

- `TARGET_AB = 4.945 V`
- `TARGET_BA = 0.280 V`

The accepted tolerances are:

- `TOL_AB = ±0.30 V`
- `TOL_BA = ±0.15 V`

LED brightness is controlled with PWM:

- `PWM_LED_GREEN = 255`
- `PWM_LED_RED = 255`

If both measurements are within range, the test result is **OK** and the green LED turns on. Otherwise, the result is **PROBLEM** and the red LED turns on.

## Hardware connections

### Cable test circuit

Use series resistors on both drive pins to limit current:

- `D8` → **1 kΩ** → cable connector 1
- `D9` → **1 kΩ** → cable connector 2
- `A0` connected to the D8-side node, after the resistor
- `A1` connected to the D9-side node, after the resistor

Recommended wiring overview:

```text
D8 --- 1 kΩ --- connector 1 ---[diode inside cable]--- connector 2 --- 1 kΩ --- D9
                 |                                                |
                A0                                               A1
```

### Status LEDs

- `D5` → 220 Ω resistor → green LED → GND
- `D6` → 220 Ω resistor → red LED → GND

## Serial monitor

At `9600 baud`, the sketch continuously prints:

- the measured voltage for `D8+,D9-`
- the measured voltage for `D9+,D8-`
- the final verdict, either `OK` or `PROBLEM`

Example:

```text
D8+,D9-: 4.940 V (target 4.945) | D9+,D8-: 0.276 V (target 0.280) => OK (green LED)
```

## Calibration

If you use another cable, diode type, supply voltage, Arduino board, or resistor value, adjust these constants in `main.ino`:

- `TARGET_AB`
- `TARGET_BA`
- `TOL_AB`
- `TOL_BA`
- `PWM_LED_GREEN` and `PWM_LED_RED` for LED brightness (`0` = off, `255` = maximum brightness)

Recommended calibration workflow:

1. Connect a known-good cable.
2. Watch the serial monitor for several measurement cycles.
3. Average the stable readings for both polarities.
4. Update `TARGET_AB` and `TARGET_BA` with those averages.
5. Adjust `TOL_AB` and `TOL_BA` wide enough to cover normal measurement variation, but narrow enough to reject bad cables.

## Project files

- `main.ino`: Arduino sketch with the bidirectional diode-test logic, calibration constants, serial output, and LED status control.
- `README.md`: English documentation for the current version of the tester.
