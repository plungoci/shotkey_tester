const uint8_t kSensePin = A0;

// Nano/UNO ADC reference voltage in millivolts when powered from 5V.
const uint16_t kReferenceMillivolts = 5000;

// Number of samples used to smooth each analog reading.
const uint8_t kSampleCount = 50;

// Voltage thresholds used for component diagnostics (in millivolts).
const uint16_t kShortThresholdMv = 100;
const uint16_t kDiodeMinThresholdMv = 150;
const uint16_t kDiodeMaxThresholdMv = 400;
const uint16_t kOpenThresholdMv = 4500;

// RGB LED is common-cathode: HIGH turns a channel ON.
const uint8_t kLedRedPin = 6;
const uint8_t kLedGreenPin = 5;
const uint8_t kLedBluePin = 3;

void setRgbLed(bool pass) {
  digitalWrite(kLedRedPin, pass ? LOW : HIGH);
  digitalWrite(kLedGreenPin, pass ? HIGH : LOW);
  digitalWrite(kLedBluePin, LOW);
}

uint16_t readAverageAdcValue() {
  uint32_t adcSum = 0;
  for (uint8_t i = 0; i < kSampleCount; i++) {
    adcSum += analogRead(kSensePin);
  }

  return adcSum / kSampleCount;
}

uint16_t adcToMillivolts(uint16_t adcValue) {
  // Rounded integer conversion: mV = adc * Vref / 1023.
  return (static_cast<uint32_t>(adcValue) * kReferenceMillivolts + 511) / 1023;
}

bool evaluateComponent(uint16_t measuredMillivolts) {
  if (measuredMillivolts < kShortThresholdMv) {
    Serial.println(F("Diag: SHORT"));
    return false;
  }

  if (measuredMillivolts >= kDiodeMinThresholdMv && measuredMillivolts <= kDiodeMaxThresholdMv) {
    Serial.println(F("Diag: DIODE OK"));
    return true;
  }

  if (measuredMillivolts > kOpenThresholdMv) {
    Serial.println(F("Diag: OPEN"));
    return false;
  }

  Serial.println(F("Diag: LEAKAGE / DEGRADATA"));
  return false;
}

void printMeasurements(uint16_t adcValue, uint16_t measuredMillivolts) {
  Serial.print(F("Measurements = "));
  Serial.print(kSampleCount);
  Serial.print(F("   Avg ADC = "));
  Serial.print(adcValue);
  Serial.print(F("   V = "));
  Serial.print(measuredMillivolts / 1000);
  Serial.print('.');

  uint16_t fractional = measuredMillivolts % 1000;
  if (fractional < 100) {
    Serial.print('0');
  }
  if (fractional < 10) {
    Serial.print('0');
  }
  Serial.println(fractional);
}

void setup() {
  Serial.begin(115200);

  pinMode(kLedRedPin, OUTPUT);
  pinMode(kLedGreenPin, OUTPUT);
  pinMode(kLedBluePin, OUTPUT);
  setRgbLed(false);
}

void loop() {
  uint16_t adcValue = readAverageAdcValue();
  uint16_t measuredMillivolts = adcToMillivolts(adcValue);

  printMeasurements(adcValue, measuredMillivolts);
  bool pass = evaluateComponent(measuredMillivolts);
  setRgbLed(pass);

  Serial.println();
  delay(500);
}
