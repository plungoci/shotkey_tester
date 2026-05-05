/*
  Test "dioda mode" pentru cablu intre D8 si D9 (Arduino Nano)
  - Masureaza tensiunea pe dioda in ambele polaritati
  - Compara cu valorile tinta:
      D8+, D9-  => ~2.265V
      D9+, D8-  => ~0.250V
  - LED verde (D5) = OK
  - LED rosu  (D6) = problema

  Conexiuni recomandate:
    D8 --- 1k --- conector 1 cablu ---[dioda in cablu]--- conector 2 --- 1k --- D9
    A0 la nodul D8 (dupa rezistor)
    A1 la nodul D9 (dupa rezistor)

    LED verde: D5 -> 220R -> LED -> GND
    LED rosu : D6 -> 220R -> LED -> GND
*/

const byte PIN_A = 8;   // D8
const byte PIN_B = 9;   // D9

const byte SENSE_A = A0; // tensiune la capatul A
const byte SENSE_B = A1; // tensiune la capatul B

const byte LED_GREEN = 5;
const byte LED_RED   = 6;

const float VCC = 5.0;

// Valorile tale "multimetru"
const float TARGET_AB = 4.945; // D8+, D9-
const float TARGET_BA = 0.280; // D9+, D8-

// Tolerante (ajusteaza daca trebuie)
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

// Returneaza caderea V(A)-V(B), pozitiva
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

// Returneaza caderea cand inversam polaritatea (D9 HIGH, D8 LOW)
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
  digitalWrite(LED_GREEN, ok ? HIGH : LOW);
  digitalWrite(LED_RED,   ok ? LOW  : HIGH);
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  setResult(false);

  hiZPins();
  delay(100);

  Serial.println("Start test tip multimetru (dioda mode)...");
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
  Serial.print(" V (tinta ");
  Serial.print(TARGET_AB, 3);
  Serial.print(") | D9+,D8-: ");
  Serial.print(v_ba, 3);
  Serial.print(" V (tinta ");
  Serial.print(TARGET_BA, 3);
  Serial.print(") => ");
  Serial.println(ok ? "OK (LED verde)" : "PROBLEMA (LED rosu)");

  delay(600);
}
