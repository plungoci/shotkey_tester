/*
  Arduino 5V
   |
 [1kΩ]
   |
   +------ A0 (analog input)
   |
  |>|  SD103B (Schottky diode)
   |
  GND
 */
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

// RGB LED is common-cathode and connected to PWM-capable pins.
const uint8_t kLedRedPin = 6;
const uint8_t kLedGreenPin = 5;
const uint8_t kLedBluePin = 3;

// LED forward voltages in millivolts (from user-provided values).
const uint16_t kLedRedForwardMv = 2000;
const uint16_t kLedGreenForwardMv = 3200;
const uint16_t kLedBlueForwardMv = 3100;

uint8_t dutyFromForwardVoltage(uint16_t forwardMv, uint16_t targetHeadroomMv) {
  if (forwardMv >= kReferenceMillivolts) {
    return 0;
  }

  uint16_t headroomMv = kReferenceMillivolts - forwardMv;
  if (headroomMv == 0) {
    return 0;
  }

  // Keep currents similar for equal resistor values on all channels:
  // duty ~= target_headroom / actual_headroom.
  uint32_t scaledDuty = (static_cast<uint32_t>(targetHeadroomMv) * 255 + (headroomMv / 2)) / headroomMv;
  if (scaledDuty > 255) {
    scaledDuty = 255;
  }

  return static_cast<uint8_t>(scaledDuty);
}

void setRgbLed(bool pass) {
  // Use the smallest headroom as reference so no channel exceeds 255.
  const uint16_t targetHeadroomMv = min(min(kReferenceMillivolts - kLedGreenForwardMv,
                                             kReferenceMillivolts - kLedBlueForwardMv),
                                        kReferenceMillivolts - kLedRedForwardMv);

  const uint8_t redDuty = dutyFromForwardVoltage(kLedRedForwardMv, targetHeadroomMv);
  const uint8_t greenDuty = dutyFromForwardVoltage(kLedGreenForwardMv, targetHeadroomMv);
  // Blue duty can be used later for mixed-color status effects.
  const uint8_t blueDuty = dutyFromForwardVoltage(kLedBlueForwardMv, targetHeadroomMv);

  // PASS = green, FAIL = red. Blue kept off for current diagnostic mode.
  analogWrite(kLedRedPin, pass ? 0 : redDuty);
  analogWrite(kLedGreenPin, pass ? greenDuty : 0);
  analogWrite(kLedBluePin, 0);
  (void)blueDuty;

  // Optional debug values (uncomment if needed):
  // Serial.print(F("PWM R/G/B = "));
  // Serial.print(redDuty);
  // Serial.print('/');
  // Serial.print(greenDuty);
  // Serial.print('/');
  // Serial.println(blueDuty);
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
