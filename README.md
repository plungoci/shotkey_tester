# Shotkey Tester

Simple Arduino-based test wiring for reading a signal on `A0` through a 1k resistor with an SD103B diode to ground.

## Wiring

```text
Arduino 5V
   |
  1k
   |
   +------ A0
   |
  |>|  SD103B
   |
  GND
```

## Files

- `main.ino` – Arduino sketch for the tester.
