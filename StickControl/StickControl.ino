#include <Arduino.h>

// Define joystick inputs
const int dpad1UpPin = 12, dpad1DownPin = 14, dpad1LeftPin = 13, dpad1RightPin = 15;
const int dpad2UpPin = 25, dpad2DownPin = 27, dpad2LeftPin = 26, dpad2RightPin = 32;
const int WeaponRelease = 16, CounterMeasures = 17, triggerPin1 = 0, triggerPin2 = 4;
const int TrimXPin = 36, TrimYPin = 39;
const int pitchHallPin = 34, rollHallPin = 35, yawHallPin = 33;
const int autoCalibrationButtonPin = 19;

const int DATA_READINGS_SIZE = 30;

// Calibration data structures
struct AxisCalibration {
  int dataReadings[DATA_READINGS_SIZE];
  int min = INT_MAX;
  int max = 0; 
  int rawNeutral = 0;
  int rawNeutralMin = INT_MAX;
  int rawNeutralMax = 0;
  int mappedMin = INT_MAX;
  int mappedMax = 0; 
  int deadZone = 0;
  int mappedValue = 0;
};

AxisCalibration pitchCal, rollCal, yawCal;

// Moving average filter data structure
struct SensorData {
    int readings[10]; // a Moving Average Filter size
    int readIndex = 0;
    long total = 0;
    int average = 0;
    int windowSize = sizeof(readings) / sizeof(readings[0]); // currently it is 10
};

SensorData pitchData, rollData, yawData;

bool calibrationCompleted = false;
unsigned long buttonPressTime = 0; // Variable to store the time button was pressed
const unsigned long longPressDuration = 3000; // 3 seconds in milliseconds
bool isButtonBeingProcessed = false; // Flag to avoid repeating the long press action

void setup() {
  Serial.begin(115200);

  pinMode(dpad1UpPin, INPUT_PULLUP);
  pinMode(dpad1DownPin, INPUT_PULLUP);
  pinMode(dpad1LeftPin, INPUT_PULLUP);
  pinMode(dpad1RightPin, INPUT_PULLUP);

  pinMode(dpad2UpPin, INPUT_PULLUP);
  pinMode(dpad2DownPin, INPUT_PULLUP);
  pinMode(dpad2LeftPin, INPUT_PULLUP);
  pinMode(dpad2RightPin, INPUT_PULLUP);

  pinMode(WeaponRelease, INPUT_PULLUP);
  pinMode(CounterMeasures, INPUT_PULLUP);

  pinMode(TrimXPin, INPUT);
  pinMode(TrimYPin, INPUT);

  pinMode(triggerPin1, INPUT_PULLUP);
  pinMode(triggerPin2, INPUT_PULLUP);

  pinMode(autoCalibrationButtonPin, INPUT_PULLUP);

  pinMode(pitchHallPin, INPUT);
  pinMode(rollHallPin, INPUT);
  pinMode(yawHallPin, INPUT);
  Serial.println("Setup complete. Waiting for auto-calibration trigger...");
}

void loop() {
  // Check if the button is pressed
  if (digitalRead(autoCalibrationButtonPin) == LOW) {
    if (buttonPressTime == 0) { // If the button was just pressed
      buttonPressTime = millis(); // Record the time button was pressed
    } else if ((millis() - buttonPressTime) >= longPressDuration && !isButtonBeingProcessed) {
      // If the button has been held for more than 3 seconds
      Serial.println("Long press detected. Starting mapped min max value measuring...");
      
      // Measure mapped data range
      measuringProcess(pitchHallPin, pitchCal, isButtonBeingProcessed); 
      Serial.println("Pitch mapped range measuring finished.");
      Serial.println("Measuring roll...");
      
      measuringProcess(rollHallPin, rollCal, isButtonBeingProcessed); 
      Serial.println("Roll mapped range measuring finished.");
      Serial.println("Measuring yaw...");

      measuringProcess(yawHallPin, yawCal, isButtonBeingProcessed); 
      Serial.println("Yaw mapped range measuring finished.");
      Serial.println("Measuring mapped values finished.");

      isButtonBeingProcessed = true; // Prevent this block from running again until button is released
      calibrationCompleted = true; // Calibration is finished
      delay(500); // Debounce delay to prevent multiple triggers
    }
  } else if (buttonPressTime > 0) { // Button was released before 3 seconds
    if (!isButtonBeingProcessed) { // Check if the long press action hasn't been triggered
      Serial.println("Starting auto-calibration...");
      autoCalibrateAxis('p'); // pitch Axis
      autoCalibrateAxis('r'); // roll Axis
      autoCalibrateAxis('y'); // yaw Axis
      calibrationCompleted = true; // Set the flag to true after calibration
    }
    // Reset variables for the next button press
    buttonPressTime = 0;
    isButtonBeingProcessed = false;
    delay(500); // Debounce delay
  }

  // After calibration, continuously read and map sensor values
  if (calibrationCompleted) {
    readAndSendValues();
    delay(10);
  }
}

void autoCalibrateAxis(char axis){
  switch (axis) {
    case 'p':
      Serial.println("Calibrating Pitch Axis...");
      calibration(pitchHallPin, pitchCal);
      Serial.println("Pitch Axis calibrated.");
      break;
    case 'r':
      Serial.println("Calibrating Roll Axis...");
      calibration(rollHallPin, rollCal);
      Serial.println("Roll Axis calibrated.");
      break;
    case 'y':
      Serial.println("Calibrating Yaw Axis...");
      calibration(yawHallPin, yawCal);
      Serial.println("Yaw Axis calibrated.");
      break;
  }
  Serial.println("Auto-calibration completed.");
}

void calibration(int pin, AxisCalibration &cal){
  // method that collects 30 units of raw data when magnet is not applied
  // and calculates the rawNeutral and deadZone
  calculatedNeutral(pin, cal);
  cal.deadZone = calculatedDeadZone(cal);

  Serial.print("rawNeutral: "); Serial.println(cal.rawNeutral);
  Serial.print("rawNeutralMin: "); Serial.println(cal.rawNeutralMin);
  Serial.print("rawNeutralMax: "); Serial.println(cal.rawNeutralMax);
  Serial.print("deadZone: "); Serial.println(cal.deadZone);

  Serial.println("Prepare to measure extreme points...");

  measuringProcess(pin, cal, true);

  Serial.print("Min point:"); Serial.println(cal.min);
  Serial.print("Max point:"); Serial.println(cal.max);
}

void calculatedNeutral(int pin, AxisCalibration &cal) {
  long sum = 0;

  // Use DATA_READINGS_SIZE to determine the number of readings
  for (int i = 0; i < DATA_READINGS_SIZE; i++) {
    int rawValue = analogRead(pin);
    cal.dataReadings[i] = rawValue;

    // Update min and max
    if (rawValue < cal.rawNeutralMin) {
      cal.rawNeutralMin = rawValue;
    }
    if (rawValue > cal.rawNeutralMax) {
      cal.rawNeutralMax = rawValue;
    }
    sum += rawValue; // Add to sum for average calculation
  }

  // Calculate and store the average
  cal.rawNeutral = sum / DATA_READINGS_SIZE;
}

int calculatedDeadZone(AxisCalibration &cal){
  return abs(cal.rawNeutralMax - cal.rawNeutralMin);
}

void measuringProcess(int pin, AxisCalibration &cal, bool buttonState){
  // Wait for the button to be released before starting the measurement process
  while(digitalRead(autoCalibrationButtonPin) == LOW);
  delay(50);
  Serial.println("Measurements started. Press the button again to finish.");
  
  unsigned long startTime = millis(); // time since the Measurements process started
  bool measuring = true;
  int mapped = 0;

  while(measuring) {
    // Continuously measures extremes
    // Sends raw and calibrated data
    if (buttonState){
      measureExtremes(pin, cal);
    } else {
      measureMappedExtremes(mapDataProcessing(analogRead(pin), cal), cal);
    }
    if(digitalRead(autoCalibrationButtonPin) == LOW) {
      delay(50);
      measuring = false; // Exit loop on button press
    }
  }

  unsigned long endTime = millis();
  Serial.println("Extreme points measurement finished.");

  // Print out the time taken and the extreme values measured
  Serial.print("The extreme values measurement process took you: ");
  Serial.print((endTime - startTime) / 1000);
  Serial.println(" seconds");
}

void measureExtremes(int pin, AxisCalibration &cal) {
  int raw = analogRead(pin);
  if (raw < cal.min) cal.min = raw;
  if (raw > cal.max) cal.max = raw;
}

void measureMappedExtremes(int pin, AxisCalibration &cal) {
  if (pin < cal.mappedMin) cal.mappedMin = pin;
  if (pin > cal.mappedMax) cal.mappedMax = pin;
}

void readAndSendValues(){
  int pitchRaw = analogRead(pitchHallPin);
  int rollRaw = analogRead(rollHallPin);
  int yawRaw = analogRead(yawHallPin);
  // Serial.print("Pitch Raw: "); Serial.println(pitchRaw);
  // Serial.print("Roll Raw: "); Serial.println(rollRaw);
  Serial.print("Yaw Raw: "); Serial.println(yawRaw);

  updateMovingAverage(pitchData, pitchRaw);
  updateMovingAverage(rollData, rollRaw);
  updateMovingAverage(yawData, yawRaw);

  // Serial.print("Pitch mapped: ");
  // pitchCal.mappedValue = mapDataProcessing(pitchData.average, pitchCal);
  // Serial.println(pitchCal.mappedValue);

  // Serial.print("Roll mapped: ");
  // rollCal.mappedValue = mapDataProcessing(rollData.average, rollCal);
  // Serial.println(rollCal.mappedValue);

  Serial.print("Yaw mapped: ");
  yawCal.mappedValue = mapDataProcessing(yawData.average, yawCal);
  Serial.println(yawCal.mappedValue);

  // Serial.print("Pitch mapped min: "); Serial.println(pitchCal.mappedMin);
  // Serial.print("Pitch mapped max: "); Serial.println(pitchCal.mappedMax);

  Serial.print("Yaw mapped min: "); Serial.println(yawCal.mappedMin);
  Serial.print("Yaw mapped max: "); Serial.println(yawCal.mappedMax);

  // Read values from all components
  int dpad1UpState = digitalRead(dpad1UpPin);
  int dpad1DownState = digitalRead(dpad1DownPin);
  int dpad1LeftState = digitalRead(dpad1LeftPin);
  int dpad1RightState = digitalRead(dpad1RightPin);

  int dpad2UpState = digitalRead(dpad2UpPin);
  int dpad2DownState = digitalRead(dpad2DownPin);
  int dpad2LeftState = digitalRead(dpad2LeftPin);
  int dpad2RightState = digitalRead(dpad2RightPin);

  int weaponReleaseState = digitalRead(WeaponRelease);
  int counterMeasuresState = digitalRead(CounterMeasures);

  int trigger1State = digitalRead(triggerPin1);
  int trigger2State = digitalRead(triggerPin2);

  byte trimXByte = mapAnalogToByte(analogRead(TrimXPin));
  byte trimYByte = mapAnalogToByte(analogRead(TrimYPin));
  
  byte pitchHallByte = mapToInt16ToByte(pitchCal.mappedValue);
  byte rollHallByte = mapToInt16ToByte(rollCal.mappedValue);
  byte yawHallByte = mapToInt16ToByte(yawCal.mappedValue);
  byte pitchMappedMinByte = mapToInt16ToByte(pitchCal.mappedMin);
  byte pitchMappedMaxByte = mapToInt16ToByte(pitchCal.mappedMax); 
  byte rollMappedMinByte = mapToInt16ToByte(rollCal.mappedMin);
  byte rollMappedMaxByte = mapToInt16ToByte(rollCal.mappedMax);
  byte yawMappedMinByte = mapToInt16ToByte(yawCal.mappedMin);
  byte yawMappedMaxByte = mapToInt16ToByte(yawCal.mappedMax);

  byte dataToSend[30] = {
    'S', 'A', 
    (byte)dpad1UpState, (byte)dpad1DownState, (byte)dpad1LeftState, (byte)dpad1RightState,
    (byte)dpad2UpState, (byte)dpad2DownState, (byte)dpad2LeftState, (byte)dpad2RightState,
    (byte)weaponReleaseState, (byte)counterMeasuresState, (byte)trigger1State, (byte)trigger2State,
    trimXByte, trimYByte, //14,15
    pitchHallByte, pitchMappedMinByte, pitchMappedMaxByte,
    rollHallByte, rollMappedMinByte, rollMappedMaxByte,
    yawHallByte, yawMappedMinByte, yawMappedMaxByte,
    0, 0, 0, 0, 'E'
  };

  Serial.write(dataToSend, sizeof(dataToSend));

  //delay(50);
}

int mapDataProcessing(int rawValue, AxisCalibration &cal){
  rawValue = constrain(rawValue, cal.min, cal.max);

  if(rawValue >= cal.rawNeutral - cal.deadZone && rawValue <= cal.rawNeutral + cal.deadZone) {
    // Closer to the neutral point, considering a tolerance range
    return 0;
  } else if(rawValue > cal.rawNeutral) {
    // Magnet influencing Side A
    return mapSensorValue(rawValue, cal.rawNeutral, cal.max, 0, 1000);
  } else {
    // Magnet influencing Side B
    return mapSensorValue(rawValue, cal.min, cal.rawNeutral, -1000, 0);
  }

  return 0;
}

long mapSensorValue(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void updateMovingAverage(SensorData &data, int newReading) {
    // Subtract the oldest reading from the total and remove it from the array
    data.total -= data.readings[data.readIndex];
    // Add the new reading to the total and the array
    data.readings[data.readIndex] = newReading;
    data.total += newReading;
    // Move to the next position in the array
    data.readIndex = (data.readIndex + 1) % data.windowSize;

    // Calculate the new average
    data.average = data.total / data.windowSize;
}

byte mapToInt16ToByte(int value) {
    // Mapping from -1000-1000 to 0-255
    return (byte)((value + 1000) * 255 / 2000);
}

byte mapAnalogToByte(int analogValue) {
    // Map from 0-4095 (analogRead range) to 0-255
    return (byte)(map(analogValue, 0, 4095, 0, 255));
}

