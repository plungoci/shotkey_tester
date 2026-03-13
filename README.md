# Shotkey Tester

`shotkey_tester` is a small Arduino-based test setup for measuring a signal on analog pin `A0`.
The circuit uses:

- a **1 kΩ series resistor** from `5V` to the measurement node, and
- an **SD103B Schottky diode** from the same node to `GND` for clamping/protection behavior.

## Electrical Wiring

### Connection summary

| From | To | Component / Notes |
| --- | --- | --- |
| Arduino `5V` | Measurement node | Through a **1 kΩ resistor** |
| Measurement node | Arduino `A0` | Direct connection for analog read |
| Measurement node | Arduino `GND` | Through **SD103B** diode |

### ASCII schematic

```text
Arduino 5V
   |
 [1kΩ]
   |
   +------ A0 (analog input)
   |
  |>|  SD103B (Schottky diode)
   |
  GND
```

### Wiring steps

1. Connect one side of the `1 kΩ` resistor to Arduino `5V`.
2. Connect the other side of the resistor to a shared **measurement node**.
3. Connect the measurement node to Arduino analog pin `A0`.
4. Connect the **anode/cathode orientation** of the SD103B as shown in the schematic between the measurement node and `GND`.
5. Verify all grounds are common before powering the board.

## Repository Contents

- `main.ino` - Arduino sketch for the tester.
- `README.md` - Project overview and electrical wiring guide.
