/*
 * Heater Control System
 * Temperature-based heating control with safety features
 * Platform: Arduino (ESP32)
 * Author: Sai
 */

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Ticker.h> // Using the Ticker for periodic tasks

// Pin Definitions
#define TEMP_SENSOR_PIN 4    // DS18B20 temperature sensor
#define HEATER_PIN 14        // Heater control
#define HEATING_LED_PIN 5    // Heating status LED
#define TARGET_LED_PIN 13    // Target reached LED
#define OVERHEAT_LED_PIN 18  // Overheat warning LED
#define BUZZER_PIN 19        // Buzzer

// Temperature Thresholds
#define TARGET_TEMP 35.0     // Target temperature
#define HYSTERESIS 2.0       // Temperature hysteresis
#define SAFETY_THRESHOLD 42.0 // Overheat protection threshold
#define STABILIZATION_TIME 30000 // Stabilization time 30sec

// System States
enum HeaterState {
  IDLE,
  HEATING,
  STABILIZING,
  TARGET_REACHED,
  OVERHEAT
};

// Global Variables
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature tempSensor(&oneWire);

HeaterState currentState = IDLE;
float currentTemp = 0.0;
bool heaterOn = false;
unsigned long lastTempRead = 0;
unsigned long stabilizationStartTime = 0;
unsigned long lastLogTime = 0;
volatile bool readTempFlag = false;

// Alarm variables
unsigned long lastAlarmToggleTime = 0;
#define ALARM_INTERVAL 200

// Ticker object
Ticker tempReaderTicker;

// Function Prototypes
void setReadTempFlag();
void readTemperature();
void updateHeaterState();
void controlHeater(bool state);
void updateStatusIndicators();
void logSystemStatus();
void handleOverheatEntry();
void manageAlarm();
bool isTemperatureStable();
String getStateString(HeaterState state);
void checkSystemHealth();


void setup() {
  Serial.begin(115200);
  Serial.println("=== Heater Control System Initializing ===");
  
  // Initialize pins
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(HEATING_LED_PIN, OUTPUT);
  pinMode(TARGET_LED_PIN, OUTPUT);
  pinMode(OVERHEAT_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Turn off all outputs initially
  digitalWrite(HEATER_PIN, LOW);
  digitalWrite(HEATING_LED_PIN, LOW);
  digitalWrite(TARGET_LED_PIN, LOW);
  digitalWrite(OVERHEAT_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Initialize temperature sensor
  tempSensor.begin();
  tempSensor.setResolution(12); // 12-bit resolution for accuracy
  
  // Setup Ticker to call setReadTempFlag every 1.0 second
  tempReaderTicker.attach(1.0, setReadTempFlag);
  
  // Initial temperature reading
  readTemperature();
  
  Serial.println("System initialized successfully!");
  Serial.println("Target Temperature: " + String(TARGET_TEMP) + "°C");
  Serial.println("Safety Threshold: " + String(SAFETY_THRESHOLD) + "°C");
  Serial.println("Hysteresis: " + String(HYSTERESIS) + "°C");
  Serial.println("=====================================");
}

void loop() {
  // Check if the Ticker has scheduled a temperature reading
  if (readTempFlag) {
    readTempFlag = false; // Reset the flag
    readTemperature();
  }
  
  // Main state machine
  updateHeaterState();
  
  // Update LED and buzzer Indicators based on the current state
  updateStatusIndicators();

  // Perform periodic safety and health checks
  checkSystemHealth();
  
  // Log system status periodically
  if (millis() - lastLogTime >= 5000) {
    logSystemStatus();
    lastLogTime = millis();
  }
}

// Callback function for the Ticker
void setReadTempFlag() {
  readTempFlag = true;
}

void readTemperature() {
  tempSensor.requestTemperatures();
  float newTemp = tempSensor.getTempCByIndex(0);
  
  // Check for sensor communication error
  if (newTemp != DEVICE_DISCONNECTED_C) {
    currentTemp = newTemp;
    lastTempRead = millis();
  } else {
    Serial.println("ERROR: Temperature sensor disconnected!");
    // A safe state is enforced by checkSystemHealth
  }
}

void updateHeaterState() {
  HeaterState previousState = currentState;
  
  // State transition logic
  switch (currentState) {
    case IDLE:
      if (currentTemp < (TARGET_TEMP - HYSTERESIS)) {
        currentState = HEATING;
      }
      break;
      
    case HEATING:
      if (currentTemp >= TARGET_TEMP) {
        currentState = STABILIZING;
        stabilizationStartTime = millis();
      }
      break;
      
    case STABILIZING:
      if (currentTemp < (TARGET_TEMP - HYSTERESIS)) {
        currentState = HEATING;
      } else if (isTemperatureStable()) {
        currentState = TARGET_REACHED;
      }
      break;
      
    case TARGET_REACHED:
      if (currentTemp < (TARGET_TEMP - HYSTERESIS)) {
        currentState = HEATING;
      }
      break;
      
    case OVERHEAT:
      if (currentTemp < (SAFETY_THRESHOLD - 5.0)) { // 5°C cooldown buffer
        currentState = IDLE;
      }
      break;
  }

  // Overheat check is prioritized over all other state logic
  if (currentTemp >= SAFETY_THRESHOLD) {
    currentState = OVERHEAT;
  }
  
  // If state has changed, log the transition
  if (previousState != currentState) {
    Serial.println("State: " + getStateString(previousState) + " -> " + getStateString(currentState));

    // Handle actions that occur on state entry
    if (currentState == OVERHEAT) {
      handleOverheatEntry();
    }
  }

  // Control heater based on the final state
  bool shouldHeat = (currentState == HEATING);
  controlHeater(shouldHeat);
}

void controlHeater(bool state) {
  if (heaterOn != state) {
    heaterOn = state;
    digitalWrite(HEATER_PIN, heaterOn ? HIGH : LOW);
    Serial.println("Heater: " + String(heaterOn ? "ON" : "OFF"));
  }
}

void updateStatusIndicators() {
  // Update LEDs based on current state
  digitalWrite(HEATING_LED_PIN, (currentState == HEATING));
  digitalWrite(TARGET_LED_PIN, (currentState == TARGET_REACHED));
  
  // Manage blinking LED and buzzer for overheat state
  if (currentState == OVERHEAT) {
    digitalWrite(OVERHEAT_LED_PIN, (millis() / 500) % 2); // Blink every 500ms
    manageAlarm();
  } else {
    digitalWrite(OVERHEAT_LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW); // Ensure buzzer is off when not overheating
  }
}

void logSystemStatus() {
  Serial.println("--- System Status ---");
  Serial.println("Temperature: " + String(currentTemp, 2) + "°C");
  Serial.println("Target: " + String(TARGET_TEMP, 1) + "°C");
  Serial.println("State: " + getStateString(currentState));
  Serial.println("Heater: " + String(heaterOn ? "ON" : "OFF"));
  Serial.println("Uptime: " + String(millis() / 1000) + "s");
  
  if (currentState == STABILIZING) {
    unsigned long stabilizationTime = millis() - stabilizationStartTime;
    Serial.println("Stabilizing: " + String(stabilizationTime / 1000) + "s / " + String(STABILIZATION_TIME / 1000) + "s");
  }
  
  Serial.println("--------------------");
}

void manageAlarm() {
  if (millis() - lastAlarmToggleTime > ALARM_INTERVAL) {
    digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN)); // Toggle buzzer state
    lastAlarmToggleTime = millis();
  }
}

void handleOverheatEntry() {
  Serial.println("!!!! OVERHEAT DETECTED! Emergency Shutdown !!!!");
  controlHeater(false); // Ensure heater is off immediately
  Serial.println("!!!! OVERHEAT ALARM !!!!!");
}

bool isTemperatureStable() {
  unsigned long elapsedStabilizationTime = millis() - stabilizationStartTime;
  
  if (elapsedStabilizationTime >= STABILIZATION_TIME) {
    return true;
  }
  
  return false;
}

String getStateString(HeaterState state) {
  switch (state) {
    case IDLE: return "IDLE";
    case HEATING: return "HEATING";
    case STABILIZING: return "STABILIZING";
    case TARGET_REACHED: return "TARGET_REACHED";
    case OVERHEAT: return "OVERHEAT";
    default: return "UNKNOWN";
  }
}

void checkSystemHealth() {
  bool isUnhealthy = false;

  // Check if temperature sensor has timed out (more than 5s without a valid read)
  if (millis() - lastTempRead > 5000) { 
    Serial.println("WARNING: Temperature sensor timeout!");
    isUnhealthy = true;
  }
  
  // Check if temperature reading is within a reasonable range
  if (currentTemp < -50.0 || currentTemp > 120.0) {
    Serial.println("WARNING: Invalid temperature reading! (" + String(currentTemp) + "°C)");
    isUnhealthy = true;
  }

  // If any health check fails, go to OVERHEAT state as a failsafe
  if (isUnhealthy && currentState != OVERHEAT) {
      Serial.println("System health check failed! Entering failsafe state.");
      currentState = OVERHEAT;
  }
}
