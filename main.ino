/*
  Cable diode-mode tester between D8 and D9 (Arduino Nano)
  - Measures the diode voltage drop in both polarities.
  - Compares the readings with the calibrated target values:
      D8+, D9-  => ~4.945 V
      D9+, D8-  => ~0.280 V
  - Green LED (D5) = OK.
  - Red LED (D6) = problem detected.

  Recommended connections:
    D8 --- 1k --- cable connector 1 ---[diode inside cable]--- connector 2 --- 1k --- D9
    A0 connected to the D8-side node, after the resistor.
    A1 connected to the D9-side node, after the resistor.

    Green LED: D5 -> 220R -> LED -> GND
    Red LED  : D6 -> 220R -> LED -> GND
*/

const byte PIN_A = 8;   // D8
const byte PIN_B = 9;   // D9

const byte SENSE_A = A0; // Voltage at cable end A
const byte SENSE_B = A1; // Voltage at cable end B

const byte LED_GREEN = 5;
const byte LED_RED   = 6;

// PWM LED brightness (0 = off, 255 = maximum brightness)
byte PWM_LED_GREEN = 255;
byte PWM_LED_RED   = 255;

const float VCC = 5.0;

// Calibrated multimeter-style target values
const float TARGET_AB = 4.945; // D8+, D9-
const float TARGET_BA = 0.280; // D9+, D8-

// Accepted tolerances; adjust if needed
const float TOL_AB = 0.30; // +/- 0.30V
const float TOL_BA = 0.15; // +/- 0.15V

float readVoltage(byte pin, byte samples = 20) {
  unsigned long sum = 0;
  for (byte i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delay(1);
  }
  float raw = (float)sum / samples;
  return raw * (VCC / 1023.0);
}

void hiZPins() {
  pinMode(PIN_A, INPUT);
  pinMode(PIN_B, INPUT);
}

// Returns the positive V(A)-V(B) voltage drop
float measureDrop_AB() {
  // D8 = HIGH, D9 = LOW
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  digitalWrite(PIN_A, HIGH);
  digitalWrite(PIN_B, LOW);
  delay(8);

  float va = readVoltage(SENSE_A);
  float vb = readVoltage(SENSE_B);
  float vd = va - vb;
  if (vd < 0) vd = -vd;
  return vd;
}

// Returns the voltage drop with reversed polarity (D9 HIGH, D8 LOW)
float measureDrop_BA() {
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  digitalWrite(PIN_A, LOW);
  digitalWrite(PIN_B, HIGH);
  delay(8);

  float va = readVoltage(SENSE_A);
  float vb = readVoltage(SENSE_B);
  float vd = vb - va;
  if (vd < 0) vd = -vd;
  return vd;
}

bool inRange(float v, float target, float tol) {
  return (v >= target - tol) && (v <= target + tol);
}

void setResult(bool ok) {
  analogWrite(LED_GREEN, ok ? PWM_LED_GREEN : 0);
  analogWrite(LED_RED,   ok ? 0             : PWM_LED_RED);
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  setResult(false);

  hiZPins();
  delay(100);

  Serial.println("Starting multimeter-style diode test...");
}

void loop() {
  float v_ab = measureDrop_AB(); // D8+, D9-
  hiZPins();
  delay(5);

  float v_ba = measureDrop_BA(); // D9+, D8-
  hiZPins();

  bool ok_ab = inRange(v_ab, TARGET_AB, TOL_AB);
  bool ok_ba = inRange(v_ba, TARGET_BA, TOL_BA);
  bool ok = ok_ab && ok_ba;

  setResult(ok);

  Serial.print("D8+,D9-: ");
  Serial.print(v_ab, 3);
  Serial.print(" V (target ");
  Serial.print(TARGET_AB, 3);
  Serial.print(") | D9+,D8-: ");
  Serial.print(v_ba, 3);
  Serial.print(" V (target ");
  Serial.print(TARGET_BA, 3);
  Serial.print(") => ");
  Serial.println(ok ? "OK (green LED)" : "PROBLEM (red LED)");

  delay(600);
}
