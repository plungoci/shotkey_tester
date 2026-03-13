const int pinSense = A0;
const float Vref = 5.0;   // pentru Uno alimentat la 5V
const int measurementCount = 50;

// RGB LED common cathode: HIGH aprinde culoarea
const int pinLedRed = 6;
const int pinLedGreen = 5;
const int pinLedBlue = 3;

void setRgbLed(bool pass) {
  digitalWrite(pinLedRed, pass ? LOW : HIGH);
  digitalWrite(pinLedGreen, pass ? HIGH : LOW);
  digitalWrite(pinLedBlue, LOW);
}

void setup() {
  Serial.begin(115200);

  pinMode(pinLedRed, OUTPUT);
  pinMode(pinLedGreen, OUTPUT);
  pinMode(pinLedBlue, OUTPUT);
  setRgbLed(false);
}

void loop() {
  long rawSum = 0;
  for (int i = 0; i < measurementCount; i++) {
    rawSum += analogRead(pinSense);
  }

  int raw = rawSum / measurementCount;
  float v = raw * Vref / 1024.0;

  Serial.print("Masuratori = ");
  Serial.print(measurementCount);
  Serial.print("   ADC mediu = ");
  Serial.print(raw);
  Serial.print("   V = ");
  Serial.println(v, 3);

  bool pass = false;
  if (v < 0.10) {
    Serial.println("Diag: SHORT");
  }
  else if (v >= 0.15 && v <= 0.40) {
    Serial.println("Diag: DIODE OK");
    pass = true;
  }
  else if (v > 4.5) {
    Serial.println("Diag: OPEN");
  }
  else {
    Serial.println("Diag: LEAKAGE / DEGRADATA");
  }

  setRgbLed(pass);

  Serial.println();
  delay(500);
}
