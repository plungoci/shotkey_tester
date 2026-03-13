const int pinSense = A0;

// Arduino UNO ADC reference voltage when powered from 5V.
const float kReferenceVoltage = 5.0;

// Number of samples used to smooth each analog reading.
const int kSampleCount = 50;

// Voltage thresholds used for component diagnostics.
const float kShortThreshold = 0.10;
const float kDiodeMinThreshold = 0.15;
const float kDiodeMaxThreshold = 0.40;
const float kOpenThreshold = 4.5;

// RGB LED is common-cathode: HIGH turns a channel ON.
const int pinLedRed = 6;
const int pinLedGreen = 5;
const int pinLedBlue = 3;

void setRgbLed(bool pass) {
  digitalWrite(pinLedRed, pass ? LOW : HIGH);
  digitalWrite(pinLedGreen, pass ? HIGH : LOW);
  digitalWrite(pinLedBlue, LOW);
}

int readAverageAdcValue() {
  long adcSum = 0;
  for (int i = 0; i < kSampleCount; i++) {
    adcSum += analogRead(pinSense);
  }

  return adcSum / kSampleCount;
}

float adcToVoltage(int adcValue) {
  return adcValue * kReferenceVoltage / 1024.0;
}

bool evaluateComponent(float measuredVoltage) {
  if (measuredVoltage < kShortThreshold) {
    Serial.println("Diag: SHORT");
    return false;
  }

  if (measuredVoltage >= kDiodeMinThreshold && measuredVoltage <= kDiodeMaxThreshold) {
    Serial.println("Diag: DIODE OK");
    return true;
  }

  if (measuredVoltage > kOpenThreshold) {
    Serial.println("Diag: OPEN");
    return false;
  }

  Serial.println("Diag: LEAKAGE / DEGRADATA");
  return false;
}

void printMeasurements(int adcValue, float measuredVoltage) {
  Serial.print("Measurements = ");
  Serial.print(kSampleCount);
  Serial.print("   Avg ADC = ");
  Serial.print(adcValue);
  Serial.print("   V = ");
  Serial.println(measuredVoltage, 3);
}

void setup() {
  Serial.begin(115200);

  pinMode(pinLedRed, OUTPUT);
  pinMode(pinLedGreen, OUTPUT);
  pinMode(pinLedBlue, OUTPUT);
  setRgbLed(false);
}

void loop() {
  int adcValue = readAverageAdcValue();
  float measuredVoltage = adcToVoltage(adcValue);

  printMeasurements(adcValue, measuredVoltage);
  bool pass = evaluateComponent(measuredVoltage);
  setRgbLed(pass);

  Serial.println();
  delay(500);
}
