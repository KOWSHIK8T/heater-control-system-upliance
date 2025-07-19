# Heater Control System using ESP32

This project is a simple yet effective Heater Control System built using an ESP32 and a DS18B20 temperature sensor. It's designed to automatically maintain a target temperature, with built-in safety features to prevent overheating.

This repository contains the full source code and documentation for the project.

**Wokwi Simulation Link:** [Click here to see it run in a simulator!](https://wokwi.com/projects/436891097299493889)

**Hardware Demo Video:** [Watch the real prototype in action!](https://drive.google.com/file/d/1sPjoh1X56OnBt9ygG4l6JknpRuhZzQ5J/view?usp=drivesdk)

---

## Features

* **Automatic Temperature Control**: Turns a heater on or off to maintain a set temperature.
* **State Machine Logic**: Uses a clear and reliable state machine (IDLE, HEATING, STABILIZING, etc.) to manage its operations.
* **Safety First**: Includes critical safety features like an overheat threshold that triggers an alarm and shuts the heater down.
* **Hysteresis**: Prevents the heater from rapidly switching on and off around the target temperature.
* **Visual & Audible Feedback**: Uses LEDs and a buzzer to clearly show the system's current status and any warnings.
* **Sensor Health Checks**: Can detect if the temperature sensor is disconnected or faulty and enter a failsafe mode.

---

## Hardware Required

* ESP32 Development Board
* DS18B20 Temperature Sensor
* 1 x Heater element (or an LED to simulate it)
* 3 x LEDs (for status indicators: Heating, Target Reached, Overheat)
* 1 x Piezo Buzzer
* 1 x 4.7kΩ Resistor (as a pull-up for the DS18B20 sensor)
* 1 x 1ch Relay
* Zero pcb, solder, Wires

---

## How It Works

The system operates based on a state machine. The current state is determined by the temperature reading from the DS18B20 sensor.

* **IDLE**: The system is waiting. If the temperature drops too low, it moves to the HEATING state.
* **HEATING**: The heater is on. It stays in this state until the target temperature is reached, then moves to STABILIZING.
* **STABILIZING**: The heater is turned off and the system waits for 30 seconds to ensure the temperature doesn't drop too quickly. If it remains stable, it moves to TARGET\_REACHED.
* **TARGET\_REACHED**: The target temperature has been successfully reached and is stable. The system monitors the temperature and will go back to HEATING if it gets too cold.
* **OVERHEAT**: A failsafe state. If the temperature exceeds the safety threshold (42°C), the heater is immediately shut down and an alarm sounds.

---

## Running the Simulation (Wokwi)

Run the project in your browser:

1. Click the Wokwi simulation link above.
2. Press the green "Start Simulation" button.
3. Interact with the sensor: click the DS18B20 and use the slider to change the temperature.
4. Watch the LEDs and serial output to observe system behavior.

---

## Software & Setup Instructions

You can set up this project using either the **Arduino IDE** or **PlatformIO**.

### Using Arduino IDE

1. **Install Libraries**:

   * `OneWire` by Paul Stoffregen
   * `DallasTemperature` by Miles Burton & others
2. **Open the Code**: `Heater_Control_System.ino`
3. **Select Board**: `ESP32 Dev Module`
4. **Upload**: Select correct COM port and click **Upload**
5. **Monitor**: Open Serial Monitor with baud rate `115200`

### Using PlatformIO (VS Code)

1. **Install PlatformIO** extension in [VS Code](https://code.visualstudio.com/)
2. **Clone Repository** and open project folder
3. **Build & Upload** using PlatformIO toolbar
4. **Monitor Output** using Serial Monitor or:

   ```bash
   pio device monitor --baud 115200
   ```

#### Example `platformio.ini`

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps =
    paulstoffregen/OneWire
    milesburton/DallasTemperature
monitor_speed = 115200
```

---

## Future Ideas & Enhancements

This project has a solid foundation. Future improvements could include:

* Add a second temperature sensor for redundancy.
* Add a **Current Sensor** (e.g., INA219) to monitor heater power.
* Enable remote control via **Wi-Fi or BLE**.
* Add **FreeRTOS** for task management and multitasking.
* Implement different heating profiles (Day, Night, Eco).
* Integrate with **Google Home / Alexa**.
* Use **machine learning** to optimize heating schedules.

---

Enjoy building and improving this smart heating system
